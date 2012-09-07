#include <cmath>

#include "cluster.h"
#include "engine.h"
#include "ratios.h"
#include "newcluster.h"
// TODO: Looks like the worker.h file is only included here,
// good reason to make it private to src.
#include "worker.h"

extern "C" {
#include "strcmp95.h"
}

//initialization of static members.
const char * const ClusterInfo::primary_delim = "###";
const char * const ClusterInfo::secondary_delim = ",";

uint32_t Worker::count = 0;
pthread_mutex_t Worker::iter_lock = PTHREAD_MUTEX_INITIALIZER;


/*
 * Aim: constructor of ClusterInfo objects
 */
ClusterInfo::ClusterInfo(const map <string, const Record*> & input_uid2record,
                         const bool input_is_matching,
                         const bool aum, //frequency adjustment
                         const bool debug)
                         : uid2record_pointer(&input_uid2record),
                           is_matching(input_is_matching),
                           frequency_adjust_mode(aum), debug_mode(debug) {

    std::cout << "A cluster information class is set up." << std::endl;
    std::cout << "FREQUENCY_ADJUST_PRIOR_MODE: " << (frequency_adjust_mode ? "ON" : "OFF")
              << "       DEBUG MODE: " << (debug_mode ? "ON" : "OFF") << std::endl;
} ;


/**
 * @return the list of clusters by the pointer of blocking id string.
 */
// cRecGroup is a list<Cluster>
const ClusterInfo::cRecGroup &
ClusterInfo::get_comparision_map(const string * bid) const {

    map<string, cRecGroup>::const_iterator q = cluster_by_block.find(*bid);
    if (q == cluster_by_block.end()) {
        throw cException_Attribute_Not_In_Tree(bid->c_str());
    }
    return q->second;
}


/**
 * @return the list of clusters by the pointer of blocking id string.
 */
ClusterInfo::cRecGroup &
ClusterInfo::get_comparision_map(const string * bid) {

    map<string, cRecGroup>::iterator q = cluster_by_block.find(*bid);
    if (q == cluster_by_block.end()) {
        throw cException_Attribute_Not_In_Tree(bid->c_str());
    }
    return q->second;
}


/**
 * Aim: to check the consistency of a ClusterInfo object
 * Algorithm: sum up the number of records after disambiguation and
 * compare it with that before disambiguation.
 * It is a very crude consistency check, and the functionality
 * can be expanded if necessary.
 */
bool
ClusterInfo::is_consistent() const {

    uint32_t temp_total = 0;
    for ( map < string, cRecGroup >::const_iterator cp = cluster_by_block.begin(); cp != cluster_by_block.end(); ++cp ) {
        for ( cRecGroup::const_iterator cq = cp->second.begin(); cq != cp ->second.end(); ++ cq ) {
            temp_total += cq->get_fellows().size();
        }
    }
    if ( temp_total != total_num )
        return false;
    return true;
}


/**
 * Aim: read the previous disambiguation results into
 * the ClusterInfo object, and block them by "blocker"
 *
 * Algorithm:
 * The records saved in the file are in the form of:
 * Delegate Unique record ID###cohesion_of_the_cluster###member1,member2,member3,...
 * where ### is the primary delimiter and , is the secondary
 * delimiter. (One can change them if necessary).
 *
 * So this function clears all the variables in the object first. And then,
 *
 *  For each line in the file:
 *      Read the delegate string, and find its Record pointer.
 *      Use the pointer and the "blocker" to create a blocking string id, b_id.
 *          For each part of b_id, record its occurrence in the variable "column_stat".
 *      Look up the map "cluster_by_block" for b_id and get the cluster list. If b_id does not exist,
 *      create insert (b_id, an empty cluster list) into cluster_by_block.
 *      Read the rest of the whole line, and create a Cluster object.
 *      Append the Cluster object into the end of the list of clusters.
 *
 *  Finally, use the variable "column_stat" to reset "min_occurrence" and "max_occurence".
 */
void
ClusterInfo::retrieve_last_comparision_info ( const cBlocking_Operation & blocker, const char * const past_comparision_file) {

    try {
        const uint32_t num_columns = blocker.num_involved_columns();

        std::ifstream::sync_with_stdio(false);
        std::ifstream infile(past_comparision_file);
        const uint32_t primary_delim_size = strlen(primary_delim);
        const uint32_t secondary_delim_size = strlen(secondary_delim);
        RecordPList empty_set;
        map < string , cRecGroup >::iterator prim_iter;
        map < const string*, map < const Record *, double> >::iterator prim_co_iter;
        uint32_t count = 0;
        const uint32_t base = 100000;

        cluster_by_block.clear();
        this->column_stat.clear();
        this->column_stat.resize(num_columns);
        this->max_occurrence.clear();
        this->max_occurrence.resize(num_columns);
        this->min_occurrence.clear();
        this->min_occurrence.resize(num_columns);

        if (infile.good()) {
            string filedata;
            cRecGroup::iterator pm;

            while ( getline(infile, filedata)) {
                register size_t pos = 0, prev_pos = 0;
                pos = filedata.find(primary_delim, prev_pos);
                string keystring = filedata.substr( prev_pos, pos - prev_pos);
                const Record * key = retrieve_record_pointer_by_unique_id( keystring, *uid2record_pointer);
                const string b_id = blocker.extract_blocking_info(key);
                vector < string > column_part (num_columns) ;

                for ( uint32_t i = 0; i < num_columns; ++i ) {
                    const string temp = blocker.extract_column_info ( key, i );
                    column_part.at(i) = temp;
                }


                prim_iter = cluster_by_block.find(b_id);

                prev_pos = pos + primary_delim_size;

                pos = filedata.find(primary_delim, prev_pos);
                double val = 0;
                if ( is_matching ) {
                    string cohesionstring = filedata.substr( prev_pos, pos - prev_pos);
                    val = atof(cohesionstring.c_str());
                }
                prev_pos = pos + primary_delim_size;


                RecordPList tempv;
                while ( ( pos = filedata.find(secondary_delim, prev_pos) )!= string::npos){
                    string valuestring = filedata.substr( prev_pos, pos - prev_pos);
                    const Record * value = retrieve_record_pointer_by_unique_id( valuestring, *uid2record_pointer);
                    tempv.push_back(value);
                    prev_pos = pos + secondary_delim_size;
                }
                ClusterHead th(key, val);
                Cluster tempc(th, tempv);
                tempc.self_repair();

                if ( prim_iter != cluster_by_block.end()) {
                    prim_iter->second.push_back(tempc);
                }
                else {
                    cRecGroup one_elem(1, tempc);
                    prim_iter = cluster_by_block.insert(std::pair<string, cRecGroup>(b_id, one_elem)).first;
                    for ( uint32_t i = 0; i < num_columns; ++i ) {
                        this->column_stat.at(i)[column_part.at(i)] += 1;
                    }
                }

                ++count;
                if ( count % base == 0 )
                    std::cout << count << " records have been loaded from the cluster file. " << std::endl;
            }

            std::cout << "Obtained ";
            for ( uint32_t i = 0; i < num_columns; ++i )
                std::cout << column_stat.at(i).size() << " / ";
            std::cout << " unique column data." << std::endl;

            uint32_t stat_cnt = 0;
            uint32_t min_stat_cnt = 0;

            for ( uint32_t i = 0; i < num_columns; ++i ) {
                stat_cnt = 0;
                for ( map<string, uint32_t >::const_iterator p = column_stat.at(i).begin(); p != column_stat.at(i).end(); ++p )
                    if ( p->second > stat_cnt && ! p->first.empty() )
                        stat_cnt = p->second;
                for ( map < string, uint32_t > ::iterator p = column_stat.at(i).begin(); p != column_stat.at(i).end(); ++p ) {
                    if ( p->second == stat_cnt )
                        std::cout << "Most common " << i << "th column part = " << p->first << " Occurrence = " << stat_cnt << std::endl;
                    if ( p->second > stat_cnt )
                        p->second = stat_cnt;
                }
                max_occurrence.at(i) = stat_cnt;

                min_stat_cnt = stat_cnt ;
                for ( map<string, uint32_t >::const_iterator p = column_stat.at(i).begin(); p != column_stat.at(i).end(); ++p )
                    if ( p->second < min_stat_cnt && ! p->first.empty() )
                        min_stat_cnt = p->second;
                min_occurrence.at(i) = min_stat_cnt;
            }

            std::cout << past_comparision_file << " has been read into memory as "
                      <<  ( is_matching ? "MATCHING" : "NON-MATCHING" )
                      << " reference." << std::endl;
        } else {

            throw cException_File_Not_Found(past_comparision_file);
        }

    } catch (const cException_Attribute_Not_In_Tree & except) {

        std::cout << " Current Unique-identifier Binary Tree, having "
                  << uid2record_pointer->size()
                  << " elements, is not complete! " << std::endl;

        std::cout << past_comparision_file
                  << " has unique identifiers that do not show in the current binary tree."
                  << std::endl;

        std::cout << except.what()
                  <<  " is missing. ( More unique identifiers are believed to be missing ) ."
                  << std::endl;

        std::cout << "Also need to check whether each row is in the format of "
                  << "<key, primary_delimiter, cohesion_value, primary delimiter, "
                  << "<<value, secondary_delimiter,>>*n >"
                  << std::endl;
        throw;
    }
}


/**
 * Aim: to read the previous disambiguation results and configure
 * them to conform to the new blocking mechanism.
 * Algorithm: call the retrieve_last comparison_info function.
 * Afterwards, reconfigure the middle names. Then count
 * the total number of records and save it.
 */
void
ClusterInfo::reset_blocking(const cBlocking_Operation & blocker, const char * const past_comparision_file) {

    total_num = 0;
    useless = blocker.get_useless_string();
    retrieve_last_comparision_info(blocker, past_comparision_file);

    for ( map <string, cRecGroup>::iterator p  = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
        for ( cRecGroup::iterator cp = p->second.begin(); cp != p->second.end(); ++cp ) {
            if ( cMiddlename::is_enabled() )
                cp->change_mid_name();
        }
    }

    for ( map <string, cRecGroup>::const_iterator p  = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
        for ( cRecGroup::const_iterator cp = p->second.begin(); cp != p->second.end(); ++cp )
            total_num += cp->get_fellows().size();
    }
}


/**
 * Aim: to consolidate records which have the same blocking id together.
 * It should be a very strict consolidation,
 * so the blocker should be very strict.
 */
void
ClusterInfo::preliminary_consolidation(const cBlocking_Operation & blocker,
                                         const list < const Record *> & all_rec_list) {

    std::cout << "Preliminary consolidation ... ..." << std::endl;
    total_num = 0;
    cluster_by_block.clear();
    useless = blocker.get_useless_string();
    map < string, cRecGroup >::iterator mi;
    const RecordPList empty_fellows;
    for ( list < const Record * > ::const_iterator p = all_rec_list.begin(); p != all_rec_list.end(); ++p ) {
        string temp ( blocker.extract_blocking_info(*p));
        mi = cluster_by_block.find(temp);
        if ( mi == cluster_by_block.end() ) {
            ClusterHead th(*p, 1);
            Cluster tc(th, empty_fellows);
            cRecGroup tr(1, tc);
            mi = cluster_by_block.insert(std::pair<string, cRecGroup>(temp, tr)).first;
        }
        mi->second.front().insert_elem(*p);
    }


    for ( mi = cluster_by_block.begin(); mi != cluster_by_block.end(); ++mi ) {
        for ( cRecGroup::iterator gi = mi->second.begin(); gi != mi->second.end(); ++gi) {
            gi->self_repair();
        }
    }

    std::cout << "Preliminary consolidation done." << std::endl;

    for ( map <string, cRecGroup>::const_iterator p  = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
        for ( cRecGroup::const_iterator cp = p->second.begin(); cp != p->second.end(); ++cp )
            total_num += cp->get_fellows().size();
    }
}


/**
 * Aim: to output "*this" ClusterInfo to an external file.
 */
void
ClusterInfo::output_current_comparision_info(const char * const outputfile ) const {

    std::ofstream of (outputfile);
    std::cout << "Dumping to " << outputfile << std::endl;
    print(of);
    std::cout << outputfile << " has been created or updated. "<< std::endl;
}


/**
 * Aim: to output "*this" ClusterInfo to an
 * ostream object. Callable internally only.
 *
 * Algorithm: for each cluster, output:
 * cluster delegate###cohesion###member1,member2,member3,...
 * where ### is the the primary delimiter and "," is the secondary delimiter.
 */
void
ClusterInfo::print(std::ostream & os) const {

    if ( is_matching && (! is_consistent() ))
        throw cException_Duplicate_Attribute_In_Tree("Not Consistent!");

    std::ostream::sync_with_stdio(false);
    const string & uid_name = cUnique_Record_ID::static_get_class_name();
    const uint32_t uid_index = Record::get_index_by_name(uid_name);
    static const cException_Vector_Data except(uid_name.c_str());

    for ( map <string, cRecGroup >::const_iterator q = cluster_by_block.begin(); q != cluster_by_block.end(); ++q ) {
        for ( cRecGroup::const_iterator p = q->second.begin(); p != q->second.end(); ++p ) {
            const Attribute * key_pattrib = p->get_cluster_head().m_delegate->get_attrib_pointer_by_index(uid_index);
            os << * key_pattrib->get_data().at(0) << primary_delim;

            double cohesion_value;
            if ( is_matching )
                cohesion_value = p->get_cluster_head().m_cohesion;
            else
                cohesion_value = 0;

            os << cohesion_value << primary_delim;

            for ( RecordPList::const_iterator q = p->get_fellows().begin(); q != p->get_fellows().end(); ++q ) {
                const Attribute * value_pattrib = (*q)->get_attrib_pointer_by_index(uid_index);
                os << * value_pattrib->get_data().at(0) << secondary_delim;
            }
            os << '\n';
        }
    }
}


/**
 * Aim: to set the activity for certain blocks, according to the input file.
 *
 * Algorithm: read the data from the input file, convert them
 * to blocking id strings, and set the corresponding blocks
 * to be active. Others remain inactive. If the input file
 * is invalid or empty, all the blocks will be set active.
 */
uint32_t
ClusterInfo::reset_block_activity( const char * const filename ) {

    const char * const delim = ClusterInfo::secondary_delim;
    uint32_t cnt = 0;
    std::cout << "Resetting block activity for debug purpose in accordance with file " << filename << " ...  " << std::endl;
    this->block_activity.clear();
    map < string , cRecGroup >::const_iterator cpm;
    for ( cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++cpm ) {
        block_activity.insert(std::pair<const string*, bool>(&cpm->first, false));
    }


    std::ifstream infile(filename);
    string data;
    while ( getline(infile, data)) {

        while(true)   {
            size_t  pos(0);
            if (   (pos = data.find(delim)) != string::npos   )
                data.replace(pos, strlen(delim), cBlocking_Operation::delim);
            else
                break;
        }
        cpm = this->cluster_by_block.find(data);
        if ( cpm == cluster_by_block.end())
            std::cout << data << " is not a good block identifier." << std::endl;
        else {
            const string * pstr = & cluster_by_block.find(data)->first;
            block_activity.find(pstr)->second = true;
            ++cnt;
        }
    }

    std::cout << "Done." << std::endl;
    if ( cnt != 0 )
        std::cout << cnt <<  " blocks have been activated." << std::endl;
    else {
        std::cout << "Warning: Since 0 blocks are active, all will be ACTIVATED instead." << std::endl;
        for ( map< const string *, bool>::iterator p = block_activity.begin(); p != block_activity.end(); ++p )
            p->second = true;
        cnt = block_activity.size();
    }
    return cnt;
}


/**
 * Aim: set up the prior value for all the blocks.
 * Algorithm: if in debug mode, only configure the relevant blocks. Otherwise, configure all the blocks.
 *         NOTE: the actual determination of priori values is by the function "get_prior_value".
 */
void ClusterInfo::config_prior()  {

    prior_data.clear();

    std::cout << "Creating prior values ..." << std::endl;

    map < const string *, bool >::const_iterator pmdebug;
    list <double> empty_list;
    map < const string *, list<double> >::iterator pp;

    for ( map<string, cRecGroup >::const_iterator cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++ cpm) {
        if ( block_activity.empty())
            break;
        const string * pstr = & cpm->first ;
        pmdebug = this->block_activity.find( pstr );
        if ( pmdebug == this->block_activity.end() )
            continue;
        else {
            if ( debug_mode && pmdebug->second == false )
                continue;
        }

        double prior = get_prior_value(cpm->first, cpm->second);

        pp = prior_data.insert(std::pair<const string*, list<double> > (& (cpm->first), empty_list)).first;
        pp->second.push_back(prior);
    }

    std::cout << "Prior values map is created." << std::endl;
}


/**
 * Aim: to output the prior values of each block to an external file. This is perfect for both analysis and debugging.
 */
void
ClusterInfo::output_prior_value( const char * const outputfile ) const {

    std::ofstream of(outputfile);
    for ( map<const string *, list<double> >::const_iterator p = prior_data.begin(); p != prior_data.end(); ++p ) {
        of << *(p->first) << " : ";
        for ( list<double>::const_iterator q = p->second.begin(); q != p->second.end(); ++q )
            of << *q << ", ";
        of << '\n';
    }
    std::cout << "Prior values are saved in " << outputfile << std::endl;
}


/**
 * Aim: the determine the priori probability for a
 * certain block, given its name and its components.
 *
 * Algorithm: according to the definition of prior values,
 * it is "the probability of match given two records that are
 * randomly selected within the block". We believe records
 * in the same cluster to be a match, and those in different clusters
 * to be a non-match. So the priori probabiliy is:
 * prior = sum ( N choose 2 ) / ( ( sum N ) choose 2 ),
 * where N is the number of members in each cluster.
 * However, we also want to penalize those frequent
 * names and offer bonus credit to rare names.
 * So we multiple the prior by log (frequency of the
 * most frequent part / frequency of the part) and
 * set it to be the new prior.
 *
 * NOTE: the adjustment function is purely arbitrary
 * and based on intuition. So one can definitely change it.
 * However, it should be noted that the adjustment is
 * crude yet very useful and effective. Those commented-out
 * notes are legacy codes of adjustment, for reference purpose only.
 */
double
ClusterInfo::get_prior_value( const string & block_identifier, const list <Cluster> & rg ) {

    static const double prior_max = 0.95;
    static const double prior_default = 1e-6;
    //attention. the uninvolved index is subject
    //to the blocking configuration. so even if
    //mid name is not a blocking part, it should
    //be in the configuration file.

    //index for middlename, which is not involved in
    //the adjustment. change to other trash value if disabled.
    const uint32_t uninvolved_index = 1; 
    std::ofstream * pfs = NULL;
    if ( debug_mode ) {
        pfs = new std::ofstream ("prior_debug.txt");
        (*pfs) << "Content size: " << '\n';
    }
    double numerator = 0;
    uint32_t tt = 0;

    for ( list<Cluster>::const_iterator q = rg.begin(); q != rg.end(); ++q ) {
        const uint32_t c = q->get_fellows().size();
        numerator += 1.0 * c * ( c - 1 );
        tt += c;

        if ( debug_mode )
            (*pfs) << c << " , ";
    }

    double denominator = 1.0 * tt * ( tt - 1 );

    if ( denominator == 0 )
        denominator = 1e10;

    double prior = numerator / denominator ;

    if ( prior == 0 )
        prior = prior_default;

    //adding frequency factor.
    //decompose the block_identifier string so as to get the frequency of each piece
    size_t pos = 0, prev_pos = 0;
    uint32_t seq = 0;
    double final_factor = 0.0;
    vector <double> factor_history;

    while ( true ) {
        pos = block_identifier.find(cBlocking_Operation::delim, prev_pos );
        if ( pos == string::npos )
            break;

        string piece = block_identifier.substr( prev_pos, pos - prev_pos );
        prev_pos = pos + cBlocking_Operation::delim.size();

        if ( seq == uninvolved_index ) {
            ++seq;
            continue;
        }
        double factor = 1.0;
        if ( frequency_adjust_mode && max_occurrence.at(seq) != 0 ) {
          //const double temp = 2.0 * log ( sqrt( 1.0 * max_occurrence.at(seq) * min_occurrence.at(seq) ) / this->column_stat.at(seq)[piece] );
          //if ( temp >= 0 ) // encourage
            //    factor = 1.0 + temp;
            //else    //panalize
            //    factor = 1.0 / ( 1.0 - temp );
            //const double temp = sqrt( 1.0 * max_occurrence.at(seq) * min_occurrence.at(seq) ) / this->column_stat.at(seq)[piece];
            //factor = sqrt(temp);
            factor = log ( 1.0 * max_occurrence.at(seq) / this->column_stat.at(seq)[piece] );

        factor_history.push_back(factor);
            //factor = 1.0 + 0.5 * ( log (  max_occurrence.at(seq) ) + log(min_occurrence.at(seq) ) ) - log ( this->column_stat.at(seq)[piece] );
        }
        if (debug_mode) {
            (*pfs) << '\n'
                   << "Part: " << seq
                   << " Max occurrence: " << max_occurrence.at(seq)
                   << " Min occurrence: " << min_occurrence.at(seq)
                   << " Self occurrence: " << this->column_stat.at(seq)[piece]
                   << " Before adjustment: "<< prior << '\n';
        }

        final_factor = max_val( final_factor,  factor );
        ++seq;
    }

    if ( final_factor <= 1 )
        prior *= final_factor;
    else {
        for ( vector <double>::const_iterator pv = factor_history.begin(); pv != factor_history.end(); ++pv )
        if ( *pv > 1 )
            prior *= *pv;
    }

    if ( debug_mode )
        (*pfs) << " After adjustment: " << prior << '\n';

    //const double first_factor = 1.0 + log ( 1.0 * max_occurrence.at(0) / this->firstname_stat[fn_piece]);
    //const double last_factor = 1.0 + log ( 1.0 * max_occurrence.at(1) / this->lastname_stat[ln_piece]);

    //prior *= first_factor * last_factor;

    if (prior > prior_max) prior = prior_max;

    if (debug_mode) delete pfs;

    return prior;
}


/**
 * Aim: to start the disambiguation.
 * Algorithm: create several thread workers and multi-thread the process.
 */
void
ClusterInfo::disambiguate(const cRatios & ratio,
		                      const uint32_t num_threads,
                          const char * const debug_block_file,
                          const char * const prior_to_save) {

    if ( is_matching_cluster() != true )
        throw cException_Cluster_Error("Wrong type of clusters for disambiguation.");

    if ( this->thresholds.empty() )
        throw cException_Cluster_Error("Thresholds have not been set up yet.");

    uint32_t size_to_disambig = this->reset_block_activity(debug_block_file);

    config_prior();

    std::cout << "Starting disambiguation ... ..." << std::endl;
    cRecGroup emptyone;
    const RecordPList emptyset;
    map<string, cRecGroup>::iterator pdisambiged;

    // now starting disambiguation.
    // here can be multithreaded.
    // variables to sync: match, nonmatch, prior_iterator, cnt.
    std::cout << "There are "<< size_to_disambig << " blocks to disambiguate." << std::endl;
    pdisambiged = cluster_by_block.begin();
    Worker sample(pdisambiged, ratio, *this);

    vector<Worker> worker_vector(num_threads, sample);

    for (uint32_t i = 0; i < num_threads; ++i) {
        worker_vector.at(i).start();
    }

    for (uint32_t i = 0; i < num_threads; ++i) {
        worker_vector.at(i).join();
    }

    std::cout << "Disambiguation done! " ;
    std::cout << Worker::get_count() << " blocks were eventually disambiguated." << std::endl;
    Worker::zero_count();

    output_prior_value(prior_to_save);


    uint32_t max_inventor = 0;
    const Cluster * pmax = NULL;

    map<string, cRecGroup >::const_iterator p = cluster_by_block.begin();
    for (; p != cluster_by_block.end(); ++p) {
        const cRecGroup & galias = p->second;

        for ( cRecGroup::const_iterator q = galias.begin(); q != galias.end(); ++q ) {
            const uint32_t t = q->get_fellows().size();

            if (t > max_inventor) {
                max_inventor = t;
                pmax = &(*q);
            }
        }
    }

    const uint32_t fi = Record::get_index_by_name(cFirstname::static_get_class_name());
    const uint32_t li = Record::get_index_by_name(cLastname::static_get_class_name());
    const uint32_t ui = Record::get_index_by_name(cUnique_Record_ID::static_get_class_name());
    std::cout << std::endl;
    std::cout << "Most consolidated cluster: " << * pmax->get_cluster_head().m_delegate->get_data_by_index(fi).at(0)
              << "." << * pmax->get_cluster_head().m_delegate->get_data_by_index(li).at(0)
              << "  ID = " << * pmax->get_cluster_head().m_delegate->get_data_by_index(ui).at(0)
              << ". Size = " << max_inventor << std::endl;
}


/**
 * Aim: thread worker of disambiguation.
 * Given the address of the cursor (iterator) of the disambiguation
 * progress, save the cursor locally and more the cursor forward.
 * Then run the disambiguation for the block to which the cursor points.
 * Finally, update the disambiguated record number.
 * PAY ATTENTION TO THE SYNCRONIZATION!
 */
void
Worker::run() {

    const uint32_t base = 10000;
    map<string, ClusterInfo::cRecGroup>::iterator pthis;

    while (true) {

        pthread_mutex_lock(&iter_lock);

        if (*ppdisambiged == cluster_ref.get_cluster_map().end() ) {
            pthread_mutex_unlock(&iter_lock);
            break;
        }

        pthis = *ppdisambiged;
        ++(*ppdisambiged);
        pthread_mutex_unlock(&iter_lock);

        bool is_success = disambiguate_wrapper(pthis, cluster_ref, *pratios);

        pthread_mutex_lock(&iter_lock);

        if (is_success) ++count;

        if (count % base == 0 && count != 0) {
            std::cout << count << " blocks have been disambiguated." << std::endl;
        }
        pthread_mutex_unlock(&iter_lock);
    }
}


void
warn_block_failure(const map<string, ClusterInfo::cRecGroup>::iterator & p) {

  std::cout << "============= POSSIBLE FAILURE IN BLOCK ==============" << std::endl;
  std::cout << p->first << " exceeded max rounds block disambiguation" << std::endl;
  std::cout << "============= END OF WARNING =============" << std::endl;
}


void
warn_very_big_blocks(const map<string, ClusterInfo::cRecGroup>::iterator & p) {

   std::cout << "Block Very Big: " << p->first
             << " Size = " << p->second.size() << std::endl;
}


void
warn_empty_block(const map<string, ClusterInfo::cRecGroup>::iterator & p) {

  std::cout << "Block Without Any Infomation Tag: " << p->first 
            << " Size = " << p->second.size() << "-----SKIPPED."
            << std::endl;
}



/**
 * Aim: a function that controls the loop of disambiguation
 * for a certain block. Used in Worker.
 */
// TODO: Strip the cruft from this function to expose what
// it's actually doing. The cruft includes emitting to cout,
// changing to semantically appropriate variable names.
// TODO: Consider rewriting this function to enforce a
// contract for valid data feeding in to the disambiguation
// loop following the validity checks. This would mean moving
// the loop out of this function.
// TODO: typedef the leading parameter, it's ugly.
bool
disambiguate_wrapper(const map<string, ClusterInfo::cRecGroup>::iterator & p,
                     ClusterInfo & cluster,
                     const cRatios & ratio ) {

    // TODO: Consider moving all validity checking to it's own
    // function.

    ///////// Start validity check /////////////

    // TODO: Rename pst for semantic utility.
    const string * pst = &p->first; // This may be "bid" elsewhere, e.g., blocking_id, tag.

    // TODO: "useless_string" should be a #define, no need to
    // drag something like "" out of memory.
    // TODO: #define USELESS_STRING ""
    // TODO: Consider doing a prepass on all the data to get rid of
    // these blocks before the disambiguation starts.
    if (p->first == cluster.get_useless_string()) {
        warn_empty_block(p);
        return false;
    }

    // TODO: alias the call in this if condition to be something
    // semantically useful, e.g., what is the thing p->second is
    // pointing to.
    if (cluster.block_activity.find(pst)->second == false) return false;

    if (p->second.size() > LARGE_BLOCK_SIZE) {
      warn_very_big_blocks(p);
    }

    /////////  End  validity check ///////


    // TODO: Rescope these variables appropriately.
    // current_size can almost surely be moved into the loop.
    uint32_t current_size = 0, new_size = 0;
    const uint32_t max_round = MAX_ROUNDS;
    // TODO: consider typedef'ing a threshold iterator:
    // typedef vector<double>::const_iterator threshit_t
    vector < double >::const_iterator c = cluster.thresholds.begin();
    for (; c != cluster.thresholds.end(); ++c) {

        uint32_t i = 0;
        for (i = 0; i < max_round; ++i) {
            // TODO: Document the logic associated with current_size and new_size
            current_size = new_size;
            // TODO: Document the return values from this method, explain why we're
            // checking the return value.
            // NOTE: the return value, here captured as size, is NOT CALCULATED in the
            // following function. What's returned from the disambiguate_by_block function
            // is a size value computed as a side effect of the called code.
            new_size = cluster.disambiguate_by_block(p->second, cluster.get_prior_map().find(pst)->second, ratio, pst, *c);
            // TODO: Explain the condition for breaking the loop here. That is,
            // why/how does size and current_size interact?
            if (new_size == current_size) break;
        }

        if (max_round == i) {
            warn_block_failure(p);
            return false;
        }
    }
    return true;
}


/**
 * Aim: set the thresholds for overall disambiguation.
 */
// TODO: Fix this.
const vector < double > &
ClusterInfo::set_thresholds ( const vector < double > & input ) {

    this->thresholds =  input;
    return this->thresholds;
}


void
ClusterInfo::debug_disambiguation_loop(cRecGroup::iterator  first_iter,
    cRecGroup::iterator second_iter,
    const double prior_value,
    const ClusterHead & result) {

  std::cout << "**************************" << std::endl;
  first_iter->get_cluster_head().m_delegate->print(std::cout);
  std::cout << "The first cluster has " << first_iter->get_fellows().size() << " members." << std::endl;
  second_iter->get_cluster_head().m_delegate->print(std::cout);
  std::cout << "The second cluster has " << second_iter->get_fellows().size() << " members." << std::endl;
  std::cout << "Prior value = "<< prior_value << " Probability = " << result.m_cohesion << std::endl;
  std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl << std::endl;
}


/**
 * Aim: to disambiguate clusters within a given block.
 * Probably also update the prior values.
 * Algorithm: call the Cluster::disambiguate method and,
 * if necessary, the Cluster::merge method.
 */
// NOTE: clusterinfo.h:257:    typedef list < Cluster > cRecGroup;
//       Expect the location of the typedef to change and the name
//       will probably also change to something like ClusterList
uint32_t /* block size, most likely */
ClusterInfo::disambiguate_by_block(cRecGroup & to_be_disambiged_group,
                                   list <double> & prior_list,
                                   const cRatios & ratio,
                                   const string * const bid, // blocking_id
                                   const double threshold ) {

    const bool should_update_prior = false;
    cRecGroup::iterator first_iter, second_iter;
    const double prior_value = prior_list.back();

    for ( first_iter = to_be_disambiged_group.begin(); first_iter != to_be_disambiged_group.end(); ++first_iter) {
        second_iter = first_iter;
        for ( ++second_iter; second_iter != to_be_disambiged_group.end(); ) {

            // TODO: Find out where the cRecGroup->iterator->disambiguate callback is set.
            // The iterator points to a Cluster object.
            ClusterHead result = first_iter->disambiguate(*second_iter, prior_value, threshold);

            // TODO: move the NULL delegate check to the debug function.
            if ( debug_mode && result.m_delegate != NULL) {
                this->debug_disambiguation_loop(first_iter, second_iter, prior_value, result);
            }

            // TODO: Well, this looks like a pretty critical piece of code...
            // merging clusters and stuff. A likely candidate for unit testing.
            if ( result.m_delegate != NULL ) {
                first_iter->merge(*second_iter, result);
                to_be_disambiged_group.erase( second_iter++ );
            }
            else
                ++second_iter;
        }
    }

    if (should_update_prior) {
        const double new_prior_value = this->get_prior_value(*bid, to_be_disambiged_group );
        if (new_prior_value != prior_value) {
            prior_list.push_back(new_prior_value);
        }
    }

    return to_be_disambiged_group.size();
}
