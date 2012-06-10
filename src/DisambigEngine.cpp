/*
 * DisambigEngine.cpp
 *
 * Created on: Dec 13, 2010
 * Author: ysun
 */

#include "DisambigEngine.h"
#include "attribute.h"
//#include "DisambigFileOper.h"
#include "DisambigCluster.h"
#include "DisambigRatios.h"
#include "DisambigNewCluster.h"
#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <numeric>

using std::map;
using std::set;



/*
 * Aim: constructors of cBlocking_Operation_Multiple_Column_Manipulate 
 * class. Look at the header file for more information.
 */
cBlocking_Operation_Multiple_Column_Manipulate::cBlocking_Operation_Multiple_Column_Manipulate (
    const vector < const StringManipulator * > & inputvsm, 
    const vector<string> & columnnames, 
    const vector < unsigned int > & di ) : vsm(inputvsm), attributes_names(columnnames) {

    if ( inputvsm.size() != columnnames.size() )
        throw cException_Other("Critical Error in cBlocking_Operation_Multiple_Column_Manipulate: size of string manipulaters is different from size of columns");
    for ( unsigned int i = 0; i < columnnames.size(); ++i ) {
        indice.push_back(Record::get_index_by_name( columnnames.at(i)));
        infoless += delim;
        pdata_indice.push_back( di.at(i));
    }
}

cBlocking_Operation_Multiple_Column_Manipulate::cBlocking_Operation_Multiple_Column_Manipulate (const StringManipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col ) {

    for ( unsigned int i = 0; i < num_col; ++i ) {
        vsm.push_back(*pinputvsm++);
        attributes_names.push_back(*pcolumnnames);
        indice.push_back(Record::get_index_by_name(*pcolumnnames++));
        infoless += delim;
        pdata_indice.push_back(*pdi ++);
    }
}

/**
 * Aim: to extract blocking information from a record pointer and 
 * returns its blocking id string.
 * 
 * Algorithm: call the polymorphic methods by StringManipulator 
 * pointers to create strings, and concatenate them.
 */
string 
cBlocking_Operation_Multiple_Column_Manipulate::extract_blocking_info(const Record * p) const {

    string temp;
    for ( unsigned int i = 0; i < vsm.size(); ++i ) {
        temp += vsm[i]->manipulate(* p->get_data_by_index(indice[i]).at(  pdata_indice.at(i)));
        temp += delim;
    }
    return temp;
};


void 
cBlocking_Operation_Multiple_Column_Manipulate::reset_data_indice ( const vector < unsigned int > & indice ) {

    if ( indice.size() != this->pdata_indice.size() )
        throw cException_Other("Indice size mismatch. cannot reset.");
    else
        this->pdata_indice = indice;
}


/*
 * Aim: to extract a specific blocking string. look at the header file for mor details.
 */
string cBlocking_Operation_Multiple_Column_Manipulate::extract_column_info ( const Record * p, unsigned int flag ) const {
    if ( flag >= indice.size() )
        throw cException_Other("Flag index error.");
    return vsm[flag]->manipulate( * p->get_data_by_index(indice[flag]).at( pdata_indice.at(flag)) );
}

/*
 * Aim: constructors of cBlocking_Operation_By_Coauthors.
 * The difference between the two constructors is that the former one builds unique record id to unique inventor id binary tree,
 * whereas the latter does not, demanding external explicit call of the building of the uid2uinv tree.
 */

cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(
    const list < const Record * > & all_rec_pointers,
    const cCluster_Info & cluster, const unsigned int coauthors)
    : patent_tree(cSort_by_attrib(cPatent::static_get_class_name())), num_coauthors(coauthors) {

    if ( num_coauthors > 4 ) {
        std::cout << "================ WARNING =====================" << std::endl;
        std::cout << "Number of coauthors in which cBlocking_Operation_By_Coauthors uses is probably too large. Number of coauthors = " << num_coauthors << std::endl;
        std::cout << "==================END OF WARNING ================" << std::endl;
    }

    build_patent_tree(all_rec_pointers);
    build_uid2uinv_tree(cluster);

    for ( unsigned int i = 0; i < num_coauthors; ++i ) {
        infoless += cBlocking_Operation::delim;
        infoless += cBlocking_Operation::delim;
    }
}



cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(const list < const Record * > & all_rec_pointers, const unsigned int coauthors)
    : patent_tree(cSort_by_attrib(cPatent::static_get_class_name())), num_coauthors(coauthors) {
    if ( num_coauthors > 4 ) {
        std::cout << "================ WARNING =====================" << std::endl;
        std::cout << "Number of coauthors in which cBlocking_Operation_By_Coauthors uses is probably too large. Number of coauthors = " << num_coauthors << std::endl;
        std::cout << "==================END OF WARNING ================" << std::endl;
    }
    build_patent_tree(all_rec_pointers);
    for ( unsigned int i = 0; i < num_coauthors; ++i ) {
        infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
        infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
    }
}


/**
 * Aim: to create a binary tree of patent -> patent holders
 * to allow fast search.
 * 
 * Algorithm: create a patent->patent holder map (std::map),
 * and for any given const Record pointer p, look for the patent
 * attribute of p in * the map. If the patent attribute is not found,
 * insert p into the map as a key, and insert a list of const Record
 * pointers which includes p as * a value of the key; if the patent
 * attribute is found, find the corresponding value ( which is a list ),
 * and append p into the list.
 */
void 
cBlocking_Operation_By_Coauthors::build_patent_tree(const list < const Record * > & all_rec_pointers) {
    map < const Record *, cGroup_Value, cSort_by_attrib >::iterator ppatentmap;
    for ( list < const Record * >::const_iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p ) {
        ppatentmap = patent_tree.find(*p);
        if ( ppatentmap == patent_tree.end() ) {
            cGroup_Value temp ( 1, *p);
            patent_tree.insert( std::pair < const Record *, cGroup_Value > (*p, temp) );
        }
        else {
            ppatentmap->second.push_back(*p);
        }
    }
}

/*
 * Aim: to create binary tree of unique record id -> unique inventor id, also to allow fast search and insertion/deletion.
 *         the unique inventor id is also a const Record pointer, meaning that different unique record ids may be associated with a same
 *         const Record pointer that represents them.
 * Algorithm: clean the uinv2count and uid2uinv tree first.
 *                 For any cluster in the cCluser_Info object:
 *                     For any const Record pointer p in the cluster member list:
 *                         create a std::pair of ( p, d ), where d is the delegate ( or representative ) of the cluster
 *                         insert the pair into uid2uinv map.
 *                     End for
 *                 End for
 *            uinv2count is updated in the same way.
 */
void cBlocking_Operation_By_Coauthors::build_uid2uinv_tree( const cCluster_Info & cluster )  {
    uinv2count_tree.clear();
    uid2uinv_tree.clear();
    unsigned int count = 0;
    typedef list<cCluster> cRecGroup;
    std::cout << "Building trees: 1. Unique Record ID to Unique Inventer ID. 2 Unique Inventer ID to Number of holding patents ........" << std::endl;
    //for ( map < string, cRecGroup >::const_iterator p = cluster.get_cluster_map().begin(); p != cluster.get_cluster_map().end(); ++p ) {
    for ( map < string, cRecGroup >::const_iterator p = cluster.get_cluster_map().begin(); p != cluster.get_cluster_map().end(); ++p ) {
        for ( cRecGroup::const_iterator q = p->second.begin(); q != p->second.end(); ++q ) {
            const Record * value = q->get_cluster_head().m_delegate;
            map < const Record *, unsigned int >::iterator pcount = uinv2count_tree.find(value);
            if ( pcount == uinv2count_tree.end() )
                pcount = uinv2count_tree.insert(std::pair<const Record *, unsigned int>(value, 0)).first;

            for ( cGroup_Value::const_iterator r = q->get_fellows().begin(); r != q->get_fellows().end(); ++r ) {
                const Record * key = *r;
                uid2uinv_tree.insert(std::pair< const Record * , const Record *> (key, value ));
                ++ ( pcount->second);
                ++count;
            }
        }
    }
    std::cout << count << " nodes has been created inside the tree." << std::endl;
}

/*
 * Aim: to get a list of top N coauthors ( as represented by a const Record pointer ) of an inventor to whom prec ( a unique record id ) belongs.
 * Algorithm:
 *         1. create a binary tree T( std::map ). Key = number of holding patents, value = const Record pointer to the unique inventor.
 *         2. For any associated record r:
 *                 find r in the uinv2count tree.
 *                 if number of nodes in T < N, insert (count(r), r) into T;
 *                 else
 *                     if count(r) > front of T:
 *                         delete front of T from T
 *                         insert ( count(r), r );
 *         3. return values in T.
 *
 */

cGroup_Value cBlocking_Operation_By_Coauthors::get_topN_coauthors( const Record * prec, const unsigned int topN ) const {
    const cGroup_Value & list_alias = patent_tree.find(prec)->second;
    map < unsigned int, cGroup_Value > occurrence_map;
    unsigned int cnt = 0;
    for ( cGroup_Value::const_iterator p = list_alias.begin(); p != list_alias.end(); ++p ) {
        if ( *p == prec )
            continue;

        map < const Record *, const Record * >::const_iterator puid2uiv = uid2uinv_tree.find(*p);
        if ( puid2uiv == uid2uinv_tree.end() )
            throw cException_Other("Critical Error: unique record id to unique inventer id tree is incomplete!!");
        const Record * coauthor_pointer = puid2uiv->second;

        map < const Record *, unsigned int >::const_iterator puinv2count = uinv2count_tree.find(coauthor_pointer);
        if ( puinv2count == uinv2count_tree.end())
            throw cException_Other("Critical Error: unique inventer id to number of holding patents tree is incomplete!!");
        const unsigned int coauthor_count = puinv2count->second;

        if ( cnt <= topN || coauthor_count > occurrence_map.begin() ->first ) {
            map < unsigned int, cGroup_Value >::iterator poccur = occurrence_map.find ( coauthor_count );
            if ( poccur == occurrence_map.end() ) {
                cGroup_Value temp (1, coauthor_pointer);
                occurrence_map.insert(std::pair<unsigned int, cGroup_Value>(coauthor_count, temp));
            }
            else
                poccur->second.push_back(coauthor_pointer);

            if ( cnt < topN )
                ++cnt;
            else {
                map < unsigned int, cGroup_Value >::iterator pbegin = occurrence_map.begin();
                pbegin->second.pop_back();
                if ( pbegin->second.empty() )
                    occurrence_map.erase(pbegin);
            }
        }
    }
    //output
    cGroup_Value ans;
    for ( map < unsigned int, cGroup_Value>::const_reverse_iterator rp = occurrence_map.rbegin(); rp != occurrence_map.rend(); ++rp )
        ans.insert(ans.end(), rp->second.begin(), rp->second.end());
    return ans;

}


/*
 * Aim: to get the blocking string id for prec.
 * Algorithm: see get_topN_coauthor
 */

string cBlocking_Operation_By_Coauthors::extract_blocking_info(const Record * prec) const {
    const cGroup_Value top_coauthor_list = get_topN_coauthors(prec, num_coauthors);
    // now make string
    const unsigned int firstnameindex = Record::get_index_by_name(cFirstname::static_get_class_name());
    const unsigned int lastnameindex = Record::get_index_by_name(cLastname::static_get_class_name());

    string answer;
    for ( cGroup_Value::const_iterator p = top_coauthor_list.begin(); p != top_coauthor_list.end(); ++p ) {
        answer += *(*p)->get_data_by_index(firstnameindex).at(0);
        answer += cBlocking_Operation::delim;
        answer += *(*p)->get_data_by_index(lastnameindex).at(0);
        answer += cBlocking_Operation::delim;
    }
    if ( answer.empty() )
        answer = infoless;
    return answer;
}



/*
 * Aim: constructor of cReconfigurator_AsianNames.
 */

cReconfigurator_AsianNames::cReconfigurator_AsianNames(): country_index(Record::get_index_by_name(cCountry::static_get_class_name())),
                                firstname_index(Record::get_index_by_name(cFirstname::static_get_class_name())),
                                middlename_index(Record::get_index_by_name(cMiddlename::static_get_class_name())),
                                lastname_index(Record::get_index_by_name(cLastname::static_get_class_name())){
    east_asian.push_back(string("KR"));
    east_asian.push_back(string("CN"));
    east_asian.push_back(string("TW"));
}


/**
 * Aim: reconfigure the asian name in accordance with the current scoring system.
 *
 * Algorithm: check the country first. If the country is in [ "KR", "CN", "TW"],
 * do this operation:
 *  1. Create a vector of string, and save FULL first name into the vector. 
 *     Then create a firstname attribute from the vector
 *  2. Create a vector of string, and save "FULL firstname" + "FULL lastname"
 *     into the vector. Then create a middlename attribute from the vector.
 *  3. Change the Record data such that the firstname pointer points to the
 *     newly created firstname object. So for the middle name.
 */
void 
cReconfigurator_AsianNames::reconfigure( const Record * p ) const {

    bool need_reconfigure = false;
    const string & country = *  p->get_attrib_pointer_by_index(country_index)->get_data().at(0) ;

    for ( register vector<string>::const_iterator ci = east_asian.begin(); ci != east_asian.end(); ++ci )
        if ( country == *ci ) {
            need_reconfigure = true;
            break;
        }

    if (need_reconfigure == false) return;

    // do not change original attributes. add new ones.
    const Attribute * cur_af = p->get_attrib_pointer_by_index(firstname_index);
    const string & fn_alias = * cur_af ->get_data().at(0);
    const vector <string> fn ( 2, fn_alias );
    const Attribute * paf = cFirstname::static_clone_by_data(fn);

    const Attribute * cur_am = p->get_attrib_pointer_by_index(middlename_index);
    const string & lnstr = * p->get_attrib_pointer_by_index(lastname_index)->get_data().at(0);
    const string mnstr ( fn_alias + "." + lnstr);
    const vector < string > mn(2, mnstr);
    const Attribute * pam = cMiddlename ::static_clone_by_data(mn);

    Record * q = const_cast < Record * > (p);

    cur_af->reduce_attrib(1);
    cur_am->reduce_attrib(1);
    q->set_attrib_pointer_by_index(paf, firstname_index);
    q->set_attrib_pointer_by_index(pam, middlename_index);
}


cReconfigurator_Interactives::cReconfigurator_Interactives( const string & my_name,
        const vector < string > & relevant_attribs ) {
    my_index = Record::get_index_by_name(my_name);
    for ( vector<string>::const_iterator p = relevant_attribs.begin(); p != relevant_attribs.end(); ++p ) {
        unsigned int idx = Record::get_index_by_name(*p);
        relevant_indice.push_back(idx);
    }
}


void 
cReconfigurator_Interactives::reconfigure ( const Record * p ) const {

    vector < const Attribute * > interact;
    for ( vector < unsigned int >::const_iterator i = relevant_indice.begin(); i != relevant_indice.end(); ++i ) {
        interact.push_back(p->get_attrib_pointer_by_index(*i));
    }
    const Attribute * const & tp = p->get_attrib_pointer_by_index(my_index);
    const Attribute * & cp = const_cast< const Attribute * &> (tp);
    cp = tp->config_interactive(interact);
}


/*
 * Aim: constructor of cReconfigurator_Coauthor
 * Algorithm: The constructor calls cCoauthor::clear_data_pool() and cCoauthor::clear_attrib_pool(). So it is critical to remember that
 * construction of each cReconfigurator_Coauthor object will DESTROY all the information of any cCoauthor class and INVALIDATE any pointers
 * that point to a cCoauthor object. Therefore, creation of cReconfigurator_Coauthor objects shall NEVER happen during disambiguation.
 *
 */
cReconfigurator_Coauthor::cReconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors) :
        reference_pointer ( & patent_authors), coauthor_index ( Record::get_index_by_name(cCoauthor::static_get_class_name())) {
    cCoauthor::clear_data_pool();
    cCoauthor::clear_attrib_pool();
}


/*
 * Aim: to recreate the whole coauthor database, and link them to appropriate pointers.
 * Algorithm: for each unique record id, find all other unique id records with which this one is associated. Then extract the FULL names
 * of those records, and save in cCoauthor data pool. After that, reset the pointer of this unique record to point to the newly built
 * attribute.
 */

void cReconfigurator_Coauthor :: reconfigure ( const Record * p ) const {
    static const string dot = ".";
    static const unsigned int firstnameindex = Record::get_index_by_name(cFirstname::static_get_class_name());
    static const unsigned int lastnameindex = Record::get_index_by_name(cLastname::static_get_class_name());
    static const StringExtractFirstWord firstname_extracter;
    static const StringRemoveSpace lastname_extracter;

    map < const Record *, cGroup_Value, cSort_by_attrib >::const_iterator cpm;
    cCoauthor temp;

    cpm = reference_pointer->find( p);
    if ( cpm == reference_pointer->end())
        throw cException_Other("Missing patent data.");
    const cGroup_Value & patent_coauthors = cpm->second;
    for ( cGroup_Value::const_iterator q = patent_coauthors.begin(); q != patent_coauthors.end(); ++q ) {
        if ( *q == p )
            continue;
        string fullname = firstname_extracter.manipulate( * (*q)->get_data_by_index(firstnameindex).at(0) ) + dot
                            + lastname_extracter.manipulate( * (*q)->get_data_by_index(lastnameindex).at(0) );
        temp.attrib_set.insert(cCoauthor::static_add_string (fullname) );
    }
    const Attribute * np = cCoauthor::static_add_attrib(temp, 1);
    const Attribute ** to_change = const_cast< const Attribute ** > ( & p->get_attrib_pointer_by_index(coauthor_index));
    *to_change = np;

}


/*
 * Aim: to find a ratio that corresponds to a given similarity
 * profile in a given similarity profile binary tree.
 * Algorithm: STL map find.
 */
double 
fetch_ratio(const vector < unsigned int > & ratio_to_lookup,
            const map < vector  < unsigned int>,
            double, cSimilarity_Compare > & ratiosmap ) {

    map < vector < unsigned int >, double, cSimilarity_Compare >::const_iterator p = ratiosmap.find( ratio_to_lookup);
    if ( p == ratiosmap.end())
        return 0;
    else
        return p->second;
}


std::pair<const Record *, double> 
disambiguate_by_set (const Record * key1,
                     const cGroup_Value & match1,
                     const double cohesion1,
                     const Record * key2,
                     const cGroup_Value & match2,
                     const double cohesion2,
                     const double prior,
                     const cRatios & ratio,  const double mutual_threshold ) {

    static const unsigned int firstname_index = Record::get_similarity_index_by_name(cFirstname::static_get_class_name());
    static const unsigned int midname_index = Record::get_similarity_index_by_name(cMiddlename::static_get_class_name());
    static const unsigned int lastname_index = Record::get_similarity_index_by_name(cLastname::static_get_class_name());
    static const unsigned int country_index = Record::get_index_by_name(cCountry::static_get_class_name());
    static const bool country_check = true;


    //prescreening.
    const bool prescreening = true;
    if ( prescreening ) {
        if ( country_check ) {
            const Attribute * p1 = key1->get_attrib_pointer_by_index(country_index);
            const Attribute * p2 = key2->get_attrib_pointer_by_index(country_index);
            if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
                return std::pair<const Record *, double> (NULL, 0);
        }


        vector < unsigned int > screen_sp = key1->record_compare(*key2);
        const double screen_r = fetch_ratio(screen_sp, ratio.get_ratios_map());
        const double screen_p = 1.0 / ( 1.0 + ( 1.0 - prior )/ prior / screen_r );
        if ( screen_p < 0.3 || screen_sp.at(firstname_index) == 0 || screen_sp.at(midname_index) == 0 || screen_sp.at(lastname_index) == 0 )
            return std::pair<const Record *, double> (NULL, 0);
    }
    const bool partial_match_mode = true;

    const double minimum_threshold = 0.7;
    const double threshold = max_val <double> (minimum_threshold, mutual_threshold * cohesion1 * cohesion2);
    static const cException_Unknown_Similarity_Profile except(" Fatal Error in Disambig by set.");

    const unsigned int match1_size = match1.size();
    const unsigned int match2_size = match2.size();

    //const unsigned int required_candidates = static_cast< unsigned int > ( 1.0 * sqrt(1.0 * match1_size * match2_size ));
    //const unsigned int candidates_for_averaging = 2 * required_candidates - 1 ;
    unsigned int candidates_for_averaging = match1_size * match2_size / 4 ;
    if ( candidates_for_averaging == 0 )
        candidates_for_averaging = 1;
    if ( candidates_for_averaging == 0 )
        throw cException_Other("Computation of size of averaged probability is incorrect.");

    set < double > probs;

    double interactive = 0;
    double cumulative_interactive = 0;
    unsigned int qualified_count = 0;
    //double required_interactives = 0;
    //unsigned int required_cnt = 0;
    for ( cGroup_Value::const_iterator p = match1.begin(); p != match1.end(); ++p ) {
        for ( cGroup_Value::const_iterator q = match2.begin(); q != match2.end(); ++q ) {

            if ( country_check ) {
                const Attribute * p1 = (*p)->get_attrib_pointer_by_index(country_index);
                const Attribute * p2 = (*q)->get_attrib_pointer_by_index(country_index);
                if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
                    return std::pair<const Record *, double> (NULL, 0);
            }



            vector< unsigned int > tempsp = (*p)->record_compare(* *q);
            if ( tempsp.at(firstname_index) == 0 || tempsp.at(midname_index) == 0 || tempsp.at(lastname_index) == 0 )
                return std::pair<const Record *, double> (NULL, 0);


            double r_value = fetch_ratio(tempsp, ratio.get_ratios_map());

            if ( r_value == 0 ) {
                interactive += 0;
            }
            else {
                const double temp_prob = 1.0 / ( 1.0 + ( 1.0 - prior )/prior / r_value );
                interactive +=  temp_prob ;
                if ( partial_match_mode && qualified_count < candidates_for_averaging ) {
                    if (  probs.size() >= candidates_for_averaging ) {
                        probs.erase(probs.begin());
                    }
                    probs.insert(temp_prob);
                }
                if ( partial_match_mode && temp_prob >= threshold ) {
                    cumulative_interactive += temp_prob;
                    ++qualified_count;
                }
            }
        }
    }

    const double interactive_average = interactive / match1_size / match2_size;
    double probs_average;

    if ( qualified_count > probs.size() )
        probs_average = cumulative_interactive / qualified_count;
    else
        probs_average = std::accumulate(probs.begin(), probs.end(), 0.0 ) / probs.size();

    if ( interactive_average > 1 )
        throw cException_Invalid_Probability("Cohesion value error.");

    if ( partial_match_mode && probs_average < threshold )
        return std::pair<const Record *, double> (NULL, probs_average);
    if ( ( ! partial_match_mode ) && interactive_average < threshold )
        return std::pair<const Record *, double> (NULL, interactive_average);


    double inter = 0;
    if ( partial_match_mode )
        inter = probs_average * match1_size * match2_size;
    else
        inter = interactive;

    const double probability = ( cohesion1 * match1_size * ( match1_size - 1 )
                                + cohesion2 * match2_size * ( match2_size - 1 )
                                + 2.0 * inter ) / ( match1_size + match2_size) / (match1_size + match2_size - 1 );
    //ATTENSION: RETURN A NON-NULL POINTER TO TELL IT IS A MERGE. NEED TO FIND A REPRESENTATIVE IN THE MERGE PART.
    return std::pair<const Record *, double>( key1, probability );

}


void 
copyfile(const char * target, const char * source) {

    std::cout << "Copying file " << source << " to " << target << std::endl;
    std::ifstream   input( source,std::ios::binary);
    std::ofstream   output( target,std::ios::binary);

    output   <<   input.rdbuf();

    std::cout << "File copy done." << std::endl;
}




/*
 * Aim: to fetch records from a txt format file into memory.
 * This is a very important function.
 *
 * Algorithm:
 * First of all, read the first line in the file. The first
 * line should include all the information of each column. ie. They
 * are usually the * column names. The format of the first line is
 * "Column Name1,Column Name 2,Column Name3,...,Column Name Last".If the delimiter is not
 * comma, change the function variable "delim".
 * Second, check the argument "requested_columns" in all the columns,
 * and record the indice of requested_columns
 * Third, starting from the second line to the end of the file,
 * read relevant information with the help of delimiters and indice,
 * and save them in appropriate attributes.
 * Finally, do some concrete class related stuff, like setting
 * static members and run reconfigurations.
 *
 */

bool
fetch_records_from_txt(list <Record> & source,
                       const char * txt_file,
                       const vector<string> &requested_columns) {

    std::ifstream::sync_with_stdio(false);
    const char * delim = ",";    // this deliminator should never occur in the data.
    const unsigned int delim_size = strlen(delim);
    std::ifstream instream(txt_file);

    if (!instream.good()) {
        throw cException_File_Not_Found(txt_file);
    }

    string line;
    //getline(instream, line);
    //if ( line != raw_txt_authenticator )
    //    throw cException_File_Not_Found("Specified file is not a valid one.");
    //std::cout << "requested_columns.size: " << requested_columns.size << std::endl; 
    std::cout << "requested_columns[0]: " << requested_columns[0] << std::endl; 

    vector <string> total_col_names;
    getline(instream, line);
    register size_t pos, prev_pos;
    pos = prev_pos = 0;

    while (  pos != string::npos){
        std::cout << "pos " << pos << std::endl;
        std::cout << "string::npos " << std::string::npos << std::endl;
        pos = line.find(delim, prev_pos);
        string columnname;
        if ( pos != string::npos )
            columnname = line.substr( prev_pos, pos - prev_pos);
        else
            columnname = line.substr( prev_pos );
        total_col_names.push_back(columnname);
        prev_pos = pos + delim_size;
        std::cout << "columnname: " << columnname << std::endl;
    }

    Attribute::register_class_names(requested_columns);
    const unsigned int num_cols = requested_columns.size();
    std::cout << "num_cols: " << num_cols << std::endl;
    vector < unsigned int > requested_column_indice;

    for ( unsigned int i = 0; i < num_cols; ++i ) {
        unsigned int j;
        for (  j = 0; j < total_col_names.size(); ++j ) {
            if ( requested_columns.at(i) == total_col_names.at(j) ) {
                requested_column_indice.push_back(j);
                break;
            }
        }
        if ( j == total_col_names.size() ) {
            std::cerr << "Critical Error in reading " << txt_file << std::endl
            <<"Column names not available in the first line. Please Check the correctness." << std::endl;
            throw cException_ColumnName_Not_Found(requested_columns.at(i).c_str());
        }
    }


    Record::column_names = requested_columns;
    Attribute ** pointer_array = new Attribute *[num_cols];

    pos = prev_pos = 0;
    unsigned int position_in_ratios = 0;

    for ( unsigned int i = 0; i < num_cols; ++i ) {

        const int pos_in_query = Attribute::position_in_registry(Record::column_names[i]);
        std::cout << "pos_in_query: " << pos_in_query << std::endl;
        std::cout << "Record::column_names[i]: " << Record::column_names[i] << std::endl;

        if ( pos_in_query == -1 ) {
            for ( unsigned int j = 0; j < i; ++j )
                delete pointer_array[j];
            delete [] pointer_array;
            throw cException_ColumnName_Not_Found(Record::column_names[i].c_str());
        } else {
            pointer_array[i] = create_attribute_instance ( Record::column_names[i].c_str() );
        }
 
#if 0
        if ( Record::column_names[i] == cLongitude::class_name ) {
            cLatitude::interactive_column_indice_in_query.push_back(i);
        }
        else if ( Record::column_names[i] == cStreet::class_name ) {
            cLatitude::interactive_column_indice_in_query.push_back(i);
        }
        else if ( Record::column_names[i] == cCountry::class_name ) {
            cLatitude::interactive_column_indice_in_query.push_back(i);
        }
        else if ( Record::column_names[i] == cAsgNum::class_name ) {
            cAssignee::interactive_column_indice_in_query.push_back(i);
        }
#endif

        // If this crashes, will need to add code in the function `create_attribute_instance`
        if ( pointer_array[i]->get_attrib_group() != string("None") ) {
            std::cout << "pointer_array[i]->get_attrib_group(): " << pointer_array[i]->get_attrib_group() << std::endl;
            ++position_in_ratios;
        }
    }

    std::cout << std::endl;

    // always do this for all the attribute classes
    for ( unsigned int i = 0; i < num_cols; ++i ) {
        pointer_array[i]->check_interactive_consistency(Record::column_names);
    }

    std::cout << "Involved attributes are: ";
    for ( unsigned int i = 0; i < num_cols; ++i )
        std::cout << pointer_array[i]->get_class_name() << ", ";
    std::cout << std::endl;

    std::cout << "Polymorphic data types are: ";
    for ( unsigned int i = 0; i < num_cols; ++i )
        std::cout << typeid(*pointer_array[i]).name()<< ", ";
    std::cout << std::endl;

    std::cout << "Polymorphic data types are: ";
    vector <string> string_cache(num_cols);
    const unsigned int string_cache_size = 2048;
    for ( unsigned int i = 0; i < num_cols; ++i ) {
        string_cache.at(i).reserve(string_cache_size);
    }

    std::cout << "Polymorphic data types are: ";
    unsigned long size = 0;
    std::cout << "Reading " << txt_file << " ......"<< std::endl;

    std::cout << "Polymorphic data types are: ";
    const unsigned int base  =  100000;
    const Attribute * pAttrib;
    vector <const Attribute *> temp_vec_attrib;
    //vector <const Attribute *> Latitude_interactive_attribute_pointers;

    // Extracts characters from ifstream (instream) and stores them 
    // into a string (line) until a delimitation character is found.
    // In this case, since a delimiter isn't given, it's assumed to be \n.
    std::cout << "Reading input data file..." << std::endl;
    while (getline(instream, line) ) {

        std::cout << "line: " << line << std::endl;

        // vector.clear is an stl method, calls all destructors
        temp_vec_attrib.clear();

        // num_cols is obtained around Line 736 above
        for ( unsigned int i = 0; i < num_cols ; ++i ) {

            // What does column_location point at?
            unsigned int column_location = 0;
            // Given the second iteration, what does prev_pos point at?
            pos = prev_pos = 0;

            // requested_column_indice is vector<unsigned int> defined
            // around Line 738 above, see vector.at
            while (column_location++ != requested_column_indice.at(i)) {
                // See string.find: http://www.cplusplus.com/reference/string/string/find/
                pos = line.find(delim, prev_pos);
                // delim_size is defined above Line ~705
                prev_pos = pos + delim_size;
            }
            pos = line.find(delim, prev_pos);

            // Find a link to string::npos
            if ( pos == string::npos ) {
                // Find a link to .size for whatever type line is
                if ( prev_pos != line.size() )
                    // Link to the substr method
                    string_cache[i] = line.substr(prev_pos);
                else
                    string_cache[i] = "";
            } else {
                // This looks where the actual value is parsed
                string_cache[i] = line.substr(prev_pos, pos - prev_pos);
            }

            // Link to the reset_data method
            pointer_array[i]->reset_data(string_cache[i].c_str());
            // Link to the clone method
            pAttrib = pointer_array[i]->clone();    //HERE CREATED NEW CLASS INSTANCES.
            temp_vec_attrib.push_back(pAttrib);
        }

        Record temprec(temp_vec_attrib);
        source.push_back( temprec );

        ++size;
        if ( size % base == 0 )
            std::cout << size << " records obtained." << std::endl;
    }
    std::cout << std::endl;
    std::cout << size << " records have been fetched from "<< txt_file << std::endl;
    Record::sample_record_pointer = & source.front();

    for ( unsigned int i = 0; i < num_cols; ++i )
        delete pointer_array[i];
    delete [] pointer_array;

    for ( list< Record>::iterator ci = source.begin(); ci != source.end(); ++ci )
        ci->reconfigure_record_for_interactives();

    std::cout << "Sample Record:---------" << std::endl;
    Record::sample_record_pointer->print();
    std::cout << "-----------------" << std::endl;

    //exit(0);

    return true;
}


Attribute *
create_attribute_instance ( const string & id ) {

    Attribute *p = NULL;
    if ( id == cFirstname::static_get_class_name() ) {
        p = new cFirstname;
    }
    else if ( id == cLastname::static_get_class_name() ) {
        p = new cLastname;
    }
    else if ( id == cMiddlename::static_get_class_name() ) {
        p = new cMiddlename;
    }
    else if ( id == cLatitude::static_get_class_name() ) {
        p = new cLatitude;
    }
    else if ( id == cLongitude::static_get_class_name() ) {
        p = new cLongitude;
    }
    else if ( id == cStreet::static_get_class_name() ) {
        p = new cStreet;
    }
    else if ( id == cState::static_get_class_name() ) {
        p = new cState;
    }
    else if ( id == cZipcode::static_get_class_name() ) {
        p = new cZipcode;
    }
    else if ( id == cInvSeq::static_get_class_name() ) {
        p = new cInvSeq;
    }
    else if ( id == cinvnum_N::static_get_class_name() ) {
        p = new cinvnum_N;
    }
    else if ( id == cinvnum::static_get_class_name() ) {
        p = new cinvnum;
    }
    //else if ( id == cAppDateStr::static_get_class_name() ) {
//        p = new cAppDateStr;
//    }
    else if ( id == cAppDate::static_get_class_name() ) {
        p = new cAppDate;
    }
    else if ( id == cCountry::static_get_class_name() ) {
        p = new cCountry;
    }
    else if ( id == cClass::static_get_class_name() ) {
        p = new cClass;
    }
    else if ( id == cCoauthor::static_get_class_name() ) {
        p = new cCoauthor;
    }
    else if ( id == cAssignee::static_get_class_name() ) {
        p = new cAssignee;
    }
    else if ( id == cAsgNum::static_get_class_name() ) {
        p = new cAsgNum;
    }
    else if ( id == cUnique_Record_ID::static_get_class_name() ) {
        p = new cUnique_Record_ID;
    }
    else if ( id == cApplyYear::static_get_class_name() ) {
        p = new cApplyYear;
    }
    else if ( id == cAppYear::static_get_class_name() ) {
        p = new cAppYear;
    }
    else if ( id == cGYear::static_get_class_name() ) {
        p = new cGYear;
    }
    else if ( id == cCity::static_get_class_name() ) {
        p = new cCity;
    }
    else if ( id == cPatent::static_get_class_name() ) {
        p = new cPatent;
    }
    else if ( id == cClass_M2::static_get_class_name() ) {
        p = new cClass_M2;
    }
    else
        p = NULL;

    return p;
}


const 
Record_Reconfigurator * generate_interactive_reconfigurator( const Attribute * pAttrib) {

    vector <string > linked_attribs (pAttrib->get_interactive_class_names());
    string my_name = pAttrib->get_class_name();
    //ATTENTION: OBJECT IS ON HEAP.
    const Record_Reconfigurator * preconfig = new cReconfigurator_Interactives( my_name, linked_attribs );
    return preconfig;
}


void 
reconfigure_interactives (const Record_Reconfigurator * pc,
                          const Record * pRec) {

    pc->reconfigure(pRec);
}


void 
cAssignee::configure_assignee( const list < const Record *> & recs) {

    static const unsigned int asgnumidx = Record::get_index_by_name(cAsgNum::static_get_class_name());

    for ( list< const Record *>::const_iterator p = recs.begin(); p != recs.end(); ++p ) {
        const cAsgNum * pasgnum = dynamic_cast < const cAsgNum *> ( (*p)->get_attrib_pointer_by_index(asgnumidx) );
        if ( ! pasgnum ) {
            throw cException_Other("Cannot perform dynamic cast to cAsgNum.");
        }
        ++cAssignee::asgnum2count_tree[pasgnum];
    }

    cAssignee::is_ready = true;
}


void 
build_patent_tree(map < const Record *, cGroup_Value, cSort_by_attrib > & patent_tree,
                        const list < const Record * > & all_rec_pointers ) {

    map < const Record *, cGroup_Value, cSort_by_attrib >::iterator ppatentmap;
    for ( list < const Record * >::const_iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p ) {
        ppatentmap = patent_tree.find(*p);
        if ( ppatentmap == patent_tree.end() ) {
            cGroup_Value temp ( 1, *p);
            patent_tree.insert( std::pair < const Record *, cGroup_Value > (*p, temp) );
        }
        else {
            ppatentmap->second.push_back(*p);
        }
    }
}


void 
build_patent_tree(map < const Record *, cGroup_Value, cSort_by_attrib > & patent_tree,
                  const list < Record > & all_records ) {

    list < const Record *> all_pointers;
    for ( list < Record >::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
        all_pointers.push_back(&(*p));
    build_patent_tree(patent_tree, all_pointers);
}


string
check_file_existence(const string & description) {

    std::ifstream infile;
    while ( true ) {
        string file;
        std::cout << "Enter file name for " << description << " : ";
        std::cin >> file;
        infile.open(file.c_str(), std::ios::in);
        if ( infile.good() ) {
            infile.close();
            std::cout << file << " accepted." << std::endl;
            return file;
        }
        else {
            std::cerr << file << " does not exist. Try again." << std::endl;
            infile.close();
        }
    }
}
