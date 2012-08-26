
#include <fstream>

#include "training.h"
#include "disambiguation.h"
#include "utilities.h"

extern "C" {
  #include "strcmp95.h"
}


cBlocking::cBlocking (const list<const Record *> & psource,
                      const vector<string> & blocking_column_names,
                      const vector<const StringManipulator*>& pmanipulators,
                      const string & unique_identifier)
                    : blocking_column_names(blocking_column_names), string_manipulator_pointers(pmanipulators) {

    const string label_delim = cBlocking_Operation::delim;
    const unsigned int num_block_columns = blocking_column_names.size();

    if (num_block_columns != pmanipulators.size()) {
        throw cBlocking::cException_Blocking("Blocking Constructor Error.");
    }

    cSort_by_attrib unique_comparator(unique_identifier);
    vector < unsigned int > blocking_indice;
    for (unsigned int i = 0; i < num_block_columns; ++i) {
        blocking_indice.push_back(Record::get_index_by_name(blocking_column_names.at(i)));
    }

    map<string, RecordPList >::iterator b_iter;
    const map<string, RecordPList >::key_compare bmap_compare = blocking_data.key_comp();
    std::cout << "Grouping ..." << std::endl;
    unsigned int count = 0;
    const unsigned int base = 100000;

    for ( list<const Record*>::const_iterator p = psource.begin(); p != psource.end(); ++p ) {
        string label;
        for ( unsigned int i = 0; i < num_block_columns; ++i ) {
            const vector < const string * > & source_data = (*p)->get_data_by_index(blocking_indice.at(i));
            if ( source_data.size() == 0 )
                throw cException_Vector_Data( (*p)->get_column_names().at(blocking_indice.at(i)).c_str());
            //for ( vector <string> :: const_iterator q = source_data.begin(); q != source_data.end(); ++q ) {
            vector < const string* > :: const_iterator q = source_data.begin();
            label += pmanipulators.at(i)->manipulate(**q);
            label += label_delim;
            //}
        }

        b_iter = blocking_data.lower_bound(label);
        if ( b_iter != blocking_data.end() && ! bmap_compare(label, b_iter->first) ) {
            //b_iter->second.insert(*p);
            b_iter->second.push_back(*p);
        }
        else {
            RecordPList tempset;
            //tempset.insert( *p );
            tempset.push_back(*p);
            b_iter = blocking_data.insert(b_iter, std::pair< string, RecordPList >(label, tempset));
        }

        record2blockingstring.insert(std::pair<const Record*, const string *>(*p, &(b_iter->first)));
        ++count;
        if ( count % base == 0 )
            std::cout << count << " records have been grouped into blocks." << std::endl;
    }
}


cBlocking_For_Training::cBlocking_For_Training(const list < const Record *> & source,
                                               const vector<string> & blocking_column_names,
                                               const vector<const StringManipulator*>& pmanipulators,
                                               const string & unique_identifier, const unsigned int qt)
                : cBlocking (source, blocking_column_names, pmanipulators, unique_identifier ), total_quota(qt) {

    reset(blocking_column_names.size());
}


void
cBlocking_For_Training::reset(const unsigned int num_cols) {

    string nullstring;
    for ( unsigned int i = 0; i < num_cols ; ++i )
        nullstring += cBlocking_Operation::delim;
    chosen_pairs.clear();
    quota_map.clear();
    used_quota_map.clear();
    outer_cursor_map.clear();
    inner_cursor_map.clear();
    unsigned long quota_distributor = 0;
    unsigned int temp_sum = 0;

    for ( map<string, RecordPList >::const_iterator cpm = blocking_data.begin(); cpm != blocking_data.end(); ++ cpm) {
        if ( cpm->first != nullstring)
            quota_distributor +=  cpm->second.size() * ( cpm->second.size() - 1 );
    }

    for ( map<string, RecordPList >::const_iterator cpm = blocking_data.begin(); cpm != blocking_data.end(); ++ cpm) {
        unsigned int quota_for_this = 0;
        if ( cpm->first != nullstring ) {
            quota_for_this = 1.0 * total_quota / quota_distributor * cpm->second.size() * ( cpm->second.size() -1 ) ;
            temp_sum += quota_for_this;
        }
        else
            quota_for_this = 0;
        const string * pstr = &cpm->first;
        quota_map.insert(std::pair<const string *, unsigned int>(pstr, quota_for_this) );
        used_quota_map.insert(std::pair<const string *, unsigned int>(pstr, 0 ) );
        RecordPList::const_iterator begin_cur = cpm->second.begin();
        outer_cursor_map.insert(std::pair < const string *, RecordPList::const_iterator > (pstr, begin_cur ) );
        inner_cursor_map.insert(std::pair < const string *, RecordPList::const_iterator > (pstr, ++begin_cur ) );
    }

    quota_left = total_quota - temp_sum;
    std::cout << "Total quota = " << total_quota << " Quota left = " << quota_left << std::endl;
    was_used = false;
}


bool
cBlocking_For_Training::move_cursor(RecordPList:: const_iterator & outer,
                                    RecordPList:: const_iterator & inner,
                                    const RecordPList & datarange) {

    while ( true ) {
        if ( outer == datarange.end() )
            return false;
        if ( inner == datarange.end() ) {
            ++ outer;
            if ( outer == datarange.end())
                return false;
            inner = outer;
        }
        ++inner;
        if ( inner != datarange.end() )
            return true;
    }
};


void
cBlocking_For_Training::print(std::ostream & os,
                              const string & unique_record_id_name) const {

    if (was_used == false) {
        throw cException_Other("Training sets are not ready to be output yet.");
    }
    PrintPair do_print(os, unique_record_id_name);
    std::for_each(chosen_pairs.begin(), chosen_pairs.end(), do_print);
}


unsigned int 
cBlocking_For_Training::create_xset01_on_block(const string & block_id,
                                               const vector <unsigned int> & equal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_equal,
                                               const vector <unsigned int> &nonequal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_nonequal,
                                               const bool is_firstround) {

    map < string, RecordPList >::const_iterator pmap = blocking_data.find(block_id);
    if ( pmap == blocking_data.end() )
        throw cException_Attribute_Not_In_Tree(block_id.c_str());
    const RecordPList & dataset = pmap->second;

    RecordPList::const_iterator & outercursor = outer_cursor_map.find(& block_id)->second;
    RecordPList::const_iterator & innercursor = inner_cursor_map.find(& block_id)->second;
    const unsigned int & quota_for_this = quota_map.find(&block_id)->second;
    unsigned int & quota_used = used_quota_map.find(&block_id)->second;


    static const unsigned int coauthors_index = Record::get_index_by_name(cCoauthor::static_get_class_name());
    static const unsigned int classes_index = Record::get_index_by_name(cClass::static_get_class_name());


    unsigned int count = 0;

    while (  outercursor != dataset.end() ) {

        if ( quota_used == quota_for_this ) {
            //std::cout << pmap->second.size() << std::endl;
            //std::cout << pmap->first << std::endl;
            return count;
        }

        bool should_continue = false;


        if ( ( ! cursor_ok ( outercursor, innercursor, dataset) && ( ! move_cursor( outercursor, innercursor, dataset ))))
            break;

        for ( unsigned int i = 0; i < equal_indice.size(); ++i) {
            const string & outerstring = * (*outercursor)->get_data_by_index(equal_indice[i]).at(0);
            const string & innerstring = * (*innercursor)->get_data_by_index(equal_indice[i]).at(0);
            if ( pmanipulators_equal[i]->manipulate( outerstring )
                    != pmanipulators_equal[i]->manipulate ( innerstring ) ) {
                should_continue = true;
                break;
            }
        }

        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

        for ( unsigned int i = 0; i < nonequal_indice.size(); ++i) {
            const string & outerstring = * (*outercursor)->get_data_by_index(nonequal_indice[i]).at(0);
            const string & innerstring = * (*innercursor)->get_data_by_index(nonequal_indice[i]).at(0);
            if ( pmanipulators_nonequal[i]->manipulate( outerstring )
                    == pmanipulators_nonequal[i]->manipulate( innerstring) ) {
                should_continue = true;
                break;
            }
        }



        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }
        //other criteria apply here.

        //std::cout << "===========" << std::endl;
        //(*outercursor)->print(std::cout);
        //(*innercursor)->print(std::cout);
        //std::cout << "===========" << std::endl;


        const Attribute * pcouter = ( (*outercursor)->get_attrib_pointer_by_index(coauthors_index));
        const Attribute * pcinner = ( (*innercursor)->get_attrib_pointer_by_index(coauthors_index));


        //set < string > outer_coauthors( (*outercursor)->get_data_by_index(coauthors_index).begin(), (*outercursor)->get_data_by_index(coauthors_index).end());

        /*debug only
        for ( set<string>::const_iterator p = outer_coauthors.begin(); p!= outer_coauthors.end(); ++p)
            std::cout << *p << " | ";
        std::cout << "----------------<- outer"<<std::endl;
        for ( vector<string>::const_iterator p = (*innercursor)->get_data_by_index(coauthors_index).begin(); p!= (*innercursor)->get_data_by_index(coauthors_index).end(); ++p)
            std::cout << *p << " | ";
        std::cout << "----------------<= inner"<<std::endl;

        */
        /*
        for ( vector<string>::const_iterator pinner = (*innercursor)->get_data_by_index(coauthors_index).begin();
                pinner != (*innercursor)->get_data_by_index(coauthors_index).end(); ++ pinner) {
            if ( outer_coauthors.find(*pinner) != outer_coauthors.end() ) {
                should_continue = true;
                break;
            }
        }
        */

        unsigned int common_coauthors = pcouter->compare(*pcinner);
        if ( common_coauthors > 0 )
            should_continue = true;

        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

        /*
        set < const string * > outer_classes ( (*outercursor)->get_data_by_index(classes_index).begin(), (*outercursor)->get_data_by_index(classes_index).end());
        for ( vector< const string*>::const_iterator pinner = (*innercursor)->get_data_by_index(classes_index).begin();
                pinner != (*innercursor)->get_data_by_index(classes_index).end(); ++ pinner) {
            if ( outer_classes.find(*pinner) != outer_classes.end() ) {
                should_continue = true;
                break;
            }
        }
        */

        pcouter = ( (*outercursor)->get_attrib_pointer_by_index(classes_index));
        pcinner = ( (*innercursor)->get_attrib_pointer_by_index(classes_index));
        unsigned int common_classes = pcouter->compare(*pcinner);
        if ( common_classes > 0 )
            should_continue = true;

        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

        chosen_pairs.push_back(RecordPairs(*outercursor, *innercursor));
        ++quota_used;
        ++count;

        move_cursor ( outercursor, innercursor, dataset );

    }
    if (is_firstround)
        quota_left += quota_for_this - count;

    return count;
}


unsigned int 
cBlocking_For_Training::create_tset05_on_block(const string & block_id,
                                               const vector <unsigned int> & equal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_equal,
                                               const vector <unsigned int> &nonequal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_nonequal,
                                               const bool is_firstround) {

    map < string, RecordPList>::const_iterator pmap = blocking_data.find(block_id);
    if ( pmap == blocking_data.end() )
        throw cException_Attribute_Not_In_Tree(block_id.c_str());
    const RecordPList & dataset = pmap->second;

    RecordPList::const_iterator & outercursor = outer_cursor_map.find(& block_id)->second;
    RecordPList::const_iterator & innercursor = inner_cursor_map.find(& block_id)->second;
    const unsigned int & quota_for_this = quota_map.find(&block_id)->second;
    unsigned int & quota_used = used_quota_map.find(&block_id)->second;

    static const unsigned int coauthors_index = Record::get_index_by_name(cCoauthor::static_get_class_name());
    static const unsigned int country_index = Record::get_index_by_name(cCountry::static_get_class_name());

    unsigned int count = 0;

    while ( outercursor != dataset.end() ) {

        if ( quota_used == quota_for_this ) {
            return count;
        }
        bool should_continue = false;
        if ( ( ! cursor_ok ( outercursor, innercursor, dataset) && ( ! move_cursor( outercursor, innercursor, dataset ))))
            break;


        for ( unsigned int i = 0; i < equal_indice.size(); ++i) {
            if ( pmanipulators_equal[i]->manipulate( * (*outercursor)->get_data_by_index(equal_indice[i]).at(0) )
                    != pmanipulators_equal[i]->manipulate ( * (*innercursor)->get_data_by_index(equal_indice[i]).at(0) ) ) {
                should_continue = true;
                break;
            }
        }

        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

        for ( unsigned int i = 0; i < nonequal_indice.size(); ++i) {
            if ( pmanipulators_nonequal[i]->manipulate( * (*outercursor)->get_data_by_index(nonequal_indice[i]).at(0) )
                    == pmanipulators_nonequal[i]->manipulate( * (*innercursor)->get_data_by_index(nonequal_indice[i]).at(0) ) ) {
                should_continue = true;
                break;
            }
        }

        static const string problem_countries[] = {"KR", "CN", "TW"};
        static const vector <string> problem_countries_vector (problem_countries, problem_countries + sizeof(problem_countries) / sizeof(string));
        const Attribute * poc = ( (*outercursor)->get_attrib_pointer_by_index(country_index));
        const Attribute * pic = ( (*innercursor)->get_attrib_pointer_by_index(country_index));

        for ( vector < string >::const_iterator pcv = problem_countries_vector.begin(); pcv != problem_countries_vector.end(); ++pcv ) {
            const string & outer_cty = * poc->get_data().at(0);
            const string & inner_cty = * pic->get_data().at(0);
            if ( outer_cty == *pcv && inner_cty == *pcv ) {
                should_continue = true;
                break;
            }
        }

        if ( should_continue ) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

            //other criteria apply here.
        unsigned int coauthors_num = 0;
        const Attribute * pcouter = ( (*outercursor)->get_attrib_pointer_by_index(coauthors_index));
        const Attribute * pcinner = ( (*innercursor)->get_attrib_pointer_by_index(coauthors_index));

        /*
        set < string > outer_coauthors( (*outercursor)->get_data_by_index(coauthors_index).begin(), (*outercursor)->get_data_by_index(coauthors_index).end());
        for ( vector<string>::const_iterator pinner = (*innercursor)->get_data_by_index(coauthors_index).begin();
                pinner != (*innercursor)->get_data_by_index(coauthors_index).end(); ++ pinner) {
            if ( outer_coauthors.find(*pinner) != outer_coauthors.end() )
                ++ coauthors_num;
            if ( coauthors_num >= 2 )
                break;
        }
        */
        coauthors_num = pcouter->compare(*pcinner);

        if ( coauthors_num >= 2 ) {
            chosen_pairs.push_back(RecordPairs(*outercursor, *innercursor));
            ++ quota_used;
            ++count;
        }

        move_cursor ( outercursor, innercursor, dataset );

    }
    if (is_firstround)
        quota_left += quota_for_this - count;

    return count;
}



unsigned int
cBlocking_For_Training::create_set(pFunc mf,
                                   const vector <string> & equal_indice_names,
                                   const vector<const StringManipulator*>& pmanipulators_equal,
                                   const vector <string> &nonequal_indice_names,
                                   const vector<const StringManipulator*>& pmanipulators_nonequal) {

    if ( was_used )
        throw cException_Other("This block has been used before. Cannot be used anymore.");
    if ( equal_indice_names.size() != pmanipulators_equal.size() )
        throw cException_Other("Creating training error: # of equal_indice_names != # of equal_string manipulators.");
    if ( nonequal_indice_names.size() != pmanipulators_nonequal.size() )
        throw cException_Other("Creating training error: # of nonequal_indice_names != # of nonequal_string manipulators.");

    std::cout << "In cBlocking_For_Training::create_set..."
              << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl;

    vector <unsigned int> equal_indice, nonequal_indice;
    for ( unsigned int i = 0; i < equal_indice_names.size(); ++i )
        equal_indice.push_back(Record::get_index_by_name(equal_indice_names[i]));
    for ( unsigned int i = 0; i < nonequal_indice_names.size(); ++i )
        nonequal_indice.push_back(Record::get_index_by_name(nonequal_indice_names[i]));

    std::cout << "Equality condition columns: ";
    for ( unsigned int i = 0; i < equal_indice_names.size(); ++i )
        std::cout << equal_indice_names[i] << ", ";
    std::cout << std::endl;
    std::cout << "Non-Equality condition columns: ";
    for ( unsigned int i = 0; i < nonequal_indice_names.size(); ++i )
        std::cout << nonequal_indice_names[i] << ", ";
    std::cout << std::endl;

    unsigned long pair_count = 0;
    const unsigned int base = 100000;
    unsigned int signal = 0;
    map < string, RecordPList >::iterator p;
    //first round: using assigned quota
    std::cout << "Obtaining training pairs ..." << std::endl;
    for ( p = blocking_data.begin(); p != blocking_data.end(); ++p){
        RecordPList::const_iterator & outercursor = outer_cursor_map.find(& p->first)->second;
        const unsigned int & quota_for_this = quota_map.find(&p->first)->second;
        const unsigned int & quota_used = used_quota_map.find(&p->first)->second;
        if ( outercursor == p->second.end() )  {
            //++p;
            continue;
        }
        else if ( quota_for_this == quota_used) {
            //++p;
            continue;
        }
        else {
            const unsigned int msize = (this->*mf)(p->first, equal_indice, pmanipulators_equal , nonequal_indice, pmanipulators_nonequal, true);
            if (msize == 0)
                continue;
            pair_count += msize;
            if ( pair_count / base != signal ) {
                signal = pair_count / base;
                std::cout << pair_count << " pairs of records are obtained for training." << std::endl;
                std::cout << "Quota for this block " << p->first << " = " << quota_for_this << " . Quota used in this block = " << quota_used << std::endl;
                std::cout << "total quota left = " << quota_left << std::endl;
            }
        }
    }
    std:: cout << "First round DONE. Obtained pair counts = " << pair_count << ". Free quota = " << quota_left << std::endl;

    //second round: using free quota.
    for ( p = blocking_data.begin(); p != blocking_data.end(); ++p) {
        RecordPList::const_iterator & outercursor = outer_cursor_map.find(& p->first)->second;
        unsigned int & quota_used = used_quota_map.find(&p->first)->second;
        if ( outercursor == p->second.end() ) {
            continue;
        }
        else {
            //std::cout << "Block: "<< p->first << " quota: " << p->second.quota
            //        << " quota used: " << p->second.quota_used << " group size = " << p->second.group_data.size() << std::endl;
            if ( quota_left < quota_used )
                quota_used -= quota_left;
            else
                quota_used = 0;
            const unsigned int msize = (this->*mf)(p->first, equal_indice,pmanipulators_equal, nonequal_indice,pmanipulators_nonequal, false);
            quota_left -= msize;
            pair_count += msize;
            if ( pair_count / base != signal ) {
                signal = pair_count / base;
                std::cout << pair_count << " pairs of records are obtained for training." << std::endl;
            }

            if ( static_cast<int>(quota_left) <= 0 )
                break;
        }
    }
    std::cout << std::endl;
    was_used = true;
    std::cout << "Second Round DONE. " << pair_count << " pairs for training are obtained." << std::endl;
    return pair_count;
}


typedef std::pair < unsigned int, unsigned int > cWord_occurrence;

void
build_word_map(const cBlocking & fullname,
    const unsigned int index, map<string, cWord_occurrence> word_map) {

    unsigned int size;
    const char * delim = " ";
    size_t position, prev_pos;
    map < string, cWord_occurrence >::iterator pword_map;

    map < string, RecordPList >::const_iterator p = fullname.get_block_map().begin();
    for (p; p != fullname.get_block_map().end() ; ++p) {
        size = p->second.size();
        position = prev_pos = 0;
        const string & info = * (*(p->second.begin()))->get_data_by_index(index).at(0);

        while (true) {

            if (0 == info.size()) break;
            position = info.find(delim, prev_pos);
            string temp = info.substr(prev_pos, position - prev_pos);
            pword_map = word_map.find(temp);

            if (pword_map == word_map.end()) {
                word_map.insert(std::pair<string, cWord_occurrence>(temp, cWord_occurrence(1, size)) );
            }

            else {
                ++(pword_map->second.first);
                pword_map->second.second += size;
            }
            if (position == string::npos) break;
            prev_pos = position + strlen(delim);
        }
    }
}


void
choose_rare_words(const map<string, cWord_occurrence> word_map, set<string> & chosen_words) {

    unsigned int num_chosen_words = 0;
    const unsigned int base = 1000;
    map<string, cWord_occurrence>::const_iterator cpword_map;

    for (cpword_map = word_map.begin(); cpword_map != word_map.end(); ++cpword_map ) {
        // TODO: Replace these magic numbers with #defines in the header.
        if (cpword_map->second.first  < RARE_NAMES_FLOOR  &&
            cpword_map->second.second > RARE_NAMES_UNIQUE &&
            cpword_map->second.second < RARE_NAMES_MAX ) {

            chosen_words.insert(cpword_map->first);
            ++num_chosen_words;
            if ( num_chosen_words % base == 0 )
                std::cout << "Number of chosen word: " << num_chosen_words << std::endl;
        }
    }

}


void
find_rare_names_v2(const vector < RecordPList * > & vec_pdest,
                   const RecordPList & source ) {

    // step 1: build phrase map: key=phrase(here is firstname+lastname with
    // some delimiters). value= list of unique_ids (here is invnums)
    const string blocks[] = {cFirstname::static_get_class_name(), cLastname::static_get_class_name()};

    const unsigned int num_columns_for_blocking = sizeof(blocks)/sizeof(string);
    const vector <string> blocking_columns(blocks, blocks + num_columns_for_blocking);
    const StringRemainSame do_not_change;
    const vector <const StringManipulator *> blocking_operator_pointers( num_columns_for_blocking, &do_not_change);
    cBlocking fullname(source, blocking_columns, blocking_operator_pointers, cUnique_Record_ID::static_get_class_name());

    std::cout << "In find_rare_names_v2..." << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl;

    //step 2: build word map:
    //key = word,
    //value = [OCCURRENCE in UNIQUE PHRASES, OCCURRENCE in ALL PHRASES]
    //split and update
    unsigned int size;
    const char * delim = " ";
    size_t position, prev_pos;
    map < string, cWord_occurrence >::iterator pword_map;
    map < string, cWord_occurrence >::const_iterator cpword_map;

    const string rarename_txt = "Rare_Names.txt";
    std::ofstream outfile(rarename_txt.c_str());
    std::cout << "Rare names are saved in the file " << rarename_txt << std::endl;

    for (unsigned int kkk = 0; kkk < vec_pdest.size(); ++kkk) {

        map < string, cWord_occurrence > word_map;
        const unsigned int cindex = Record::get_index_by_name(blocking_columns[kkk]);
        build_word_map(fullname, cindex, word_map);

        //step 3: find words whose unique phrase occurrence is
        //low but total occurrence is not too low.
        set <string> chosen_words;
        choose_rare_words(word_map, chosen_words);

        //step 4: find the name part that contains the words.
        outfile << blocking_columns.at(kkk) << ":" << '\n';


        unsigned int num_chosen_words = 0;
        const unsigned int base = 1000;
        set <string> in_phrase_wordset;

        map < string, RecordPList >::const_iterator p = fullname.get_block_map().begin();
        for (; p != fullname.get_block_map().end() ; ++p) {
            const string & info = * (*(p->second.begin()))->get_data_by_index(cindex).at(0);
            in_phrase_wordset.clear();
            position = prev_pos = 0;
            do {
                position = info.find(delim, prev_pos);
                string temp;
                if ( position != string::npos)
                    temp = info.substr(prev_pos, position - prev_pos);
                else
                    temp = info.substr(prev_pos);
                in_phrase_wordset.insert(temp);
                prev_pos = position + strlen(delim);
            } while ( position != string::npos);

            for (set<string>::const_iterator pm = in_phrase_wordset.begin(); pm != in_phrase_wordset.end(); ++pm) {
                if ( chosen_words.find(*pm) != chosen_words.end() ) {
                    vec_pdest[kkk]->push_back(*p->second.begin());
                    outfile << * (*p->second.begin())->get_data_by_index(cindex).at(0) << " , ";
                    ++ num_chosen_words;
                    if (0 == (num_chosen_words % base)) {
                        std::cout << "Number of chosen phrases: " << num_chosen_words << std::endl;
                    }
                    break;
                }
            }
        }
        std::cout << "Number of chosen phrases: "<< vec_pdest.at(kkk)->size() << std::endl;
        outfile << '\n';
    }
}


// TODO: Unit test this thing
unsigned int
create_tset02(list <RecordPairs> & results,
              const list <const Record*> & reclist,
              const vector <string> & column_names,
              const vector < const RecordPList * > & vec_prare_names,
              const unsigned int limit) {

    //equal_indice should be the indices of the names in this case.
    if (column_names.size() > vec_prare_names.size()) {
        std::cout << "ERROR: Number of input column names > size of vector of rare names." << std::endl;
        throw cException_Other("Wrong column names");
    }

    std::cout << "In create_tset02..." << __FILE__ << ":"
              << STRINGIZE(__LINE__) << std::endl;

    vector < unsigned int > column_indice;

    vector<string>::const_iterator p = column_names.begin();
    for (; p != column_names.end(); ++p) {
        column_indice.push_back(Record::get_index_by_name(*p));
    }

    RecordPList emptyone;
    map <string, RecordPList >::iterator pm;
    map <string, RecordPList >::const_iterator cpm;
    unsigned int count = 0;

    set < RecordPairs > answer;
    const unsigned int base = 100000;
    for (unsigned int i = 0; i < column_names.size(); ++i) {

        const unsigned int col_index = column_indice.at(i);
        map < string, RecordPList > data_map;

        RecordPList::const_iterator p = vec_prare_names[i]->begin();
        for (; p != vec_prare_names[i]->end(); ++p ) {
            data_map.insert(std::pair<string, RecordPList >( * (*p)->get_data_by_index(col_index).at(0), emptyone));
        }

        list<const Record*>::const_iterator q = reclist.begin();
        for (; q != reclist.end(); ++q) {
            pm = data_map.find( * (*q)->get_data_by_index(col_index).at(0));
            if (pm != data_map.end()) pm->second.push_back(*q);
        }

        for (cpm = data_map.begin(); cpm != data_map.end(); ++cpm) {

            RecordPList::const_iterator rr = cpm->second.begin();
            for (; rr != cpm->second.end(); ++rr ) {
                RecordPList::const_iterator ss = rr;
                ++ss;
                for (; ss != cpm->second.end(); ++ss) {
                    bool data_ok = true;
                    for (unsigned int j = 0; j < column_indice.size(); ++j) {
                        if ( (*rr)->get_data_by_index(column_indice.at(j)) != (*ss)->get_data_by_index(column_indice.at(j)) ) {
                            data_ok = false;
                            break;
                        }
                    }

                    if (data_ok) {
                        answer.insert(RecordPairs( *rr, *ss));
                        ++count;
                        if (count % base == 0) {
                            std::cout << "Tset02: " << count << " records obtained." << std::endl;
                        }

                        if (count >= limit) {
                            results.insert(results.begin(), answer.begin(), answer.end());
                            return count;
                        }
                    }
                }
            }
        }

    }

    results.insert(results.begin(), answer.begin(), answer.end());
    return count;
}


// http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml?showone=Function_Parameter_Ordering#Function_Parameter_Ordering
/**
 * Called from utilities.cpp
 * @return results
 */
unsigned int
create_xset03(list <RecordPairs> & results,
              //const list <const Record*> & reclist,
              const vector < const RecordPList * > & vec_prare_names,
              const unsigned int limit ) {

    RecordPList pool;
    for (unsigned int i = 0; i < vec_prare_names.size(); ++i) {
        pool.insert(pool.end(), vec_prare_names.at(i)->begin(), vec_prare_names.at(i)->end());
    }

    std::cout << "In create_xset03..." << __FILE__
              << ":" << STRINGIZE(__LINE__) << std::endl;

    unsigned int count = 0;
    const unsigned int base = 100000;

    RecordPList::const_iterator p = pool.begin();
    for (; p != pool.end(); ++p) {

        RecordPList::const_iterator q = p;
        for (++q; q != pool.end(); ++q) {
            results.push_back(RecordPairs(*p, *q));
            ++count;
            if (count % base == 0) {
                std::cout << "Xset03: " << count << " records obtained." << std::endl;
            }
            if (count >= limit) {
                return count;
            }
        }
    }

    return count;
}


// TODO: Call this file from `create_xset03`
void
write_xset03(const char * current_file, list<RecordPairs> pair_list) {

    std::ofstream outfile;
    outfile.open(current_file);

    if (!outfile.good() ) {
        exit_with_error("bad file stream", __FILE__, STRINGIZE(__LINE__));
    }

    std::cout << "Creating " << current_file << " ..."
              << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl;

    PrintPair do_print(outfile, cUnique_Record_ID::static_get_class_name());
    std::for_each(pair_list.begin(), pair_list.end(), do_print);
    outfile.close();
    std::cout << "Done" << std::endl;
}



void
write_tset02(const char * current_file, list<RecordPairs> pair_list) {

    std::ofstream outfile;
    outfile.open(current_file);
    if (!outfile.good()) {
        exit_with_error("bad file stream", __FILE__, STRINGIZE(__LINE__));
    }

    std::cout << "Creating " << current_file << " ..."
              << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl;

    PrintPair do_print(outfile, cUnique_Record_ID::static_get_class_name());
    std::for_each(pair_list.begin(), pair_list.end(), do_print);
    outfile.close();
    std::cout << "Done" << std::endl;
}


// Is this is the first and/or only place a RecordPList is
// created? If not, we're almost surely leaking memory...
// RecordList should be its own class, which can be queried
// for a list of pointers to records.
void
create_record_plist(const list<Record> & rl,  RecordPList & rpl) {

    list<Record>::const_iterator p = rl.begin();
    for (; p != rl.end(); ++p) {
        rpl.push_back(&(*p));
    }
}


// TODO: Move to training.cpp
bool
make_stable_training_sets_by_personal(const list <Record> & all_records,
                                      const unsigned int limit,
                                      const vector <string> & training_filenames) {

    //if ( training_filenames.size() != 2 )
        //throw cException_Other("Training: there should be 2 changeable training sets.");

    RecordPList rare_firstname_set;
    RecordPList rare_lastname_set;

    const char * current_file;
    vector<RecordPList *> rare_pointer_vec;
    rare_pointer_vec.push_back(&rare_firstname_set);
    rare_pointer_vec.push_back(&rare_lastname_set);
    const vector< const RecordPList * > const_rare_pointer_vec(rare_pointer_vec.begin(), rare_pointer_vec.end());

    RecordPList record_pointers;
    create_record_plist(all_records, record_pointers);

    // rare_pointer_vec is output...?
    find_rare_names_v2(rare_pointer_vec, record_pointers);
    vector <string> rare_column_names;
    rare_column_names.push_back(string(cFirstname::static_get_class_name()));
    rare_column_names.push_back(string(cLastname::static_get_class_name()));

    list<RecordPairs> pair_list;
    pair_list.clear();
    // TODO: Unit test this, pair_list is probably output
    create_xset03(pair_list, /*record_pointers,*/ const_rare_pointer_vec, limit);
    current_file = training_filenames.at(0).c_str();
    write_xset03(current_file, pair_list);

    pair_list.clear();
    // TODO: Create a unit test for this
    create_tset02(pair_list, record_pointers, rare_column_names, const_rare_pointer_vec, limit);
    current_file = training_filenames.at(1).c_str();
    write_tset02(current_file, pair_list);

    return true;
}


unsigned int
cBlocking_For_Training::create_xset03_on_block(const string & block_id,
                                               const vector <unsigned int> & equal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_equal,
                                               const vector <unsigned int> &nonequal_indice,
                                               const vector<const StringManipulator*>& pmanipulators_nonequal,
                                               const bool is_firstround) {

    map < string, RecordPList >::const_iterator pmap = blocking_data.find(block_id);
    if ( pmap == blocking_data.end() )
        throw cException_Attribute_Not_In_Tree(block_id.c_str());
    const RecordPList & dataset = pmap->second;

    RecordPList::const_iterator & outercursor = outer_cursor_map.find(& block_id)->second;
    RecordPList::const_iterator & innercursor = inner_cursor_map.find(& block_id)->second;
    const unsigned int & quota_for_this = quota_map.find(&block_id)->second;
    unsigned int & quota_used = used_quota_map.find(&block_id)->second;

    unsigned int count = 0;

    while (outercursor != dataset.end()) {

        if (quota_used == quota_for_this) {
            return count;
        }

        bool should_continue = false;

        if ( ( ! cursor_ok ( outercursor, innercursor, dataset) && ( ! move_cursor( outercursor, innercursor, dataset ))))
            break;


        for (unsigned int i = 0; i < equal_indice.size(); ++i) {
            if ( (*outercursor)->get_data_by_index(equal_indice[i]).at(0) != (*innercursor)->get_data_by_index(equal_indice[i]).at(0) ) {
                should_continue = true;
                break;
            }
        }

        if (should_continue) {
            move_cursor ( outercursor, innercursor, dataset );
            continue;
        }

        for (unsigned int i = 0; i < nonequal_indice.size(); ++i) {
            if ( (*outercursor)->get_data_by_index(nonequal_indice[i]).at(0) == (*innercursor)->get_data_by_index(nonequal_indice[i]).at(0) ) {
                should_continue = true;
                break;
            }
        }

        if (should_continue) {
            move_cursor (outercursor, innercursor, dataset);
            continue;
        }

            //other criteria apply here.
        bool should_select = true;
        //this is the lastname comparison part.
        for ( unsigned int i = 0; i < nonequal_indice.size(); ++i) {
            if ( strcmp95_modified ((*outercursor)->get_data_by_index( nonequal_indice[i] ).at(0)->c_str(),
                                    (*innercursor)->get_data_by_index( nonequal_indice[i] ).at(0)->c_str() ) > 0.8 ) {
                should_select = false;
                break;
            }
        }

        if (should_select) {
            chosen_pairs.push_back(RecordPairs(*outercursor, *innercursor));
            ++ quota_used;
            ++count;
        }

        move_cursor (outercursor, innercursor, dataset);
    }

    if (is_firstround) quota_left += quota_for_this - count;

    return count;
}


pthread_mutex_t cWorker_For_Training::iter_mutex = PTHREAD_MUTEX_INITIALIZER;

void cWorker_For_Training::run() {
}


unsigned int
create_xset01(list <RecordPairs> &results, const list <const Record *> & source,
              const unsigned int limit) {

    //std::cout << " Creating xset01 ..." << std::endl;
    std::cout << "In create_xset01..." << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl;
    results.clear();
    unsigned int count = 0;
    const unsigned int base = 100000;
    StringRemainSame operator_no_change;
    const vector <string> blocking_column_names(1, cPatent::static_get_class_name());
    const vector < const StringManipulator*> pman(1, &operator_no_change);
    //const unsigned int fnidx = Record::get_index_by_name(cFirstname::static_get_class_name());
    //const unsigned int lnidx = Record::get_index_by_name(cLastname::static_get_class_name());
    cBlocking bl(source, blocking_column_names, pman, cPatent::static_get_class_name());

    for ( map < string, RecordPList>::const_iterator pm = bl.get_block_map().begin(); pm != bl.get_block_map().end(); ++pm ) {
        for ( RecordPList::const_iterator p = pm->second.begin(); p != pm->second.end(); ++p ) {
            RecordPList::const_iterator q = p;
            for ( ++q; q != pm->second.end(); ++q ) {
                // now ok
                results.push_back(RecordPairs(*p, *q) );
                ++count;
                if ( count % base == 0)
                    std::cout << "xset01: " << count << " training pairs have been acquired." << std::endl;
                if ( count == limit ) {
                    std::cout << " xset01 done." << std::endl;
                    return count;
                }
            }
        }
    }
    std::cout << " xset01 done." << std::endl;
    return count;
}

