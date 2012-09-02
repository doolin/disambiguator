/** @file */

#ifndef PATENT_TRAINING_H
#define PATENT_TRAINING_H

#define RARE_NAMES_FLOOR  4
#define RARE_NAMES_UNIQUE 6
#define RARE_NAMES_MAX    100


/**
 * Training:
 *
 * xset01: non-match set of record pairs to train personal
 * information based on patent information
 * xset01 is directly from each patent. By building a
 * patent tree, it is obtained easily.
 *
 * xset03: non-match set of record pairs to train patent
 * information based on personal information
 *
 * xset03 and tset02 are both from the rare name list.
 * See find_rarename_v2 function for more information
 */

/**
 * tset02: match set of record pairs to train patent information
 * based on personal information
 *
 * tset05: match set of record pairs to train personal
 * information based on patent information
 */


/**
 * tset05 is the only part that depends on the blocking mechanism,
 * therefore, the cBlocking part should be consistent with the
 * cBlocking_Operation object used for blocking.
 *
 * Actually, one can rewrite the whole DisambigTraining.h/cpp code
 * to abandon the cBlocking class, which is from legacy codes.
 */

/**
 *
 * Due to the limit of record pairs in each training set
 * (> 1 million and <10 million in our case), in order
 * to obtain an unbiased training set, two rounds of pair
 * selections are involved. First, a certain quota, proportional
 * to the size of each block (actually the square of the
 * size of the block) is decided when pairs are chosen from
 * the block. Then pairs are chosen from blocks until their
 * quota are used up. This process starts from the first block
 * and ends till the last * block. Second, after that, if
 * there is any residue quota for the whole database, the
 * residue quota will be filled up by taking the rest
 * possible pairs starting from the first block, until
 * all the quota are used up.
 */


#include "attribute.h"
#include "engine.h"
#include "threading.h"



typedef std::pair< const Record *, const Record *> RecordPairs;

struct PrintPair {

private:
    vector < unsigned int > indice;
    unsigned int single_index;
    std::ostream & myos;

    const char * primary_delim;
    const char * secondary_delim;
    const bool is_vector;

public:

    PrintPair(std::ostream & os, const vector < string > & vec_columnnames)
        : myos(os), primary_delim(","), secondary_delim( " | "), is_vector(true) {

        vector<string>::const_iterator p = vec_columnnames.begin();
        for (; p != vec_columnnames.end() ; ++p) {
            indice.push_back(Record::get_index_by_name(*p));
        }
    }

    PrintPair(std::ostream & os, const string & col_name)
            : myos(os), primary_delim(","), secondary_delim( " | "), is_vector(false) {
        single_index = Record::get_index_by_name(col_name);
    }

    void operator() (const RecordPairs & source) {

        if (is_vector) {

            for (vector< unsigned int>::const_iterator p = indice.begin(); p != indice.end(); ++p) {
                myos << * source.first->get_data_by_index(*p).at(0) << secondary_delim;
            }

            myos << primary_delim;

            for (vector< unsigned int>::const_iterator p = indice.begin(); p != indice.end(); ++p) {
                myos << * source.second->get_data_by_index(*p).at(0) << secondary_delim;
            }

            myos << '\n';
        }
        else {
            myos << * source.first->get_data_by_index(single_index).at(0) << primary_delim
                 << * source.second->get_data_by_index(single_index).at(0)<<'\n';
        }
    }
};


class cException_Reach_Limit: public cAbstract_Exception {
public:
    cException_Reach_Limit(const char* errmsg): cAbstract_Exception(errmsg) {};
};



class cBlocking {

//    friend class cCluster_Info;

private:
    cBlocking ( const cBlocking &);

protected:

    class cException_Blocking: public cAbstract_Exception {
    public:
        cException_Blocking (const char * errmsg) : cAbstract_Exception(errmsg) {};
    };

    class cString_Pointer_Compare{
    public:
        bool operator() (const string * p1, const string *p2) const {
            return (*p1) < (*p2);
        }
    };

    class cException_Tree_Key_Mismatch : public cAbstract_Exception {
    public:
        cException_Tree_Key_Mismatch(const char* errmsg): cAbstract_Exception(errmsg) {};
    };

    map<string, RecordPList > blocking_data;

    map < const Record *, const string *> record2blockingstring;

    const vector <string> blocking_column_names;

    const vector<const StringManipulator*> string_manipulator_pointers;

public:

    explicit cBlocking(const list<const Record *> & psource, const vector<string> & blocking_column_names,
        const vector<const StringManipulator*>& pmanipulators, const string & unique_identifier);

    const map<string, RecordPList > & get_block_map() const {return blocking_data;}
};



class cBlocking_For_Training : public cBlocking {

private:

    map<const string *, unsigned int, cBlocking::cString_Pointer_Compare> quota_map;

    map<const string *, unsigned int, cBlocking::cString_Pointer_Compare> used_quota_map;

    map<const string *, RecordPList::const_iterator, cBlocking::cString_Pointer_Compare> outer_cursor_map;

    map<const string *, RecordPList::const_iterator, cBlocking::cString_Pointer_Compare> inner_cursor_map;

    const unsigned int total_quota;

    unsigned int quota_left;

    bool was_used;

    list <RecordPairs> chosen_pairs;

    bool move_cursor(RecordPList::const_iterator & outer,
        RecordPList:: const_iterator & inner, const RecordPList & datarange);

    bool cursor_ok(const RecordPList::const_iterator & outer,
        const RecordPList:: const_iterator & inner, const RecordPList & datarange) const {
        return  ( outer != datarange.end() && inner != datarange.end() &&  inner != outer );
    }

public:
    typedef unsigned int(cBlocking_For_Training::*pFunc)(const string & block_id,
        const vector <unsigned int> & equal_indice,
        const vector<const StringManipulator*>& pmanipulators_equal,
        const vector <unsigned int> &nonequal_indice,
        const vector<const StringManipulator*>& pmanipulators_nonequal,
        const bool is_firstround);

    explicit cBlocking_For_Training(const list < const Record *> & source,
        const vector<string> & blocking_column_names,
        const vector<const StringManipulator*>& pmanipulators,
        const string & unique_identifier, const unsigned int qt);

    unsigned int create_xset01_on_block(const string & block_id,
        const vector <unsigned int> & equal_indice,
        const vector<const StringManipulator*>& pmanipulators_equal,
        const vector <unsigned int> &nonequal_indice,
        const vector<const StringManipulator*>& pmanipulators_nonequal,
        const bool is_firstround);

    unsigned int create_tset05_on_block(const string & block_id,
        const vector <unsigned int> & equal_indice,
        const vector<const StringManipulator*>& pmanipulators_equal,
        const vector <unsigned int> &nonequal_indice,
        const vector<const StringManipulator*>& pmanipulators_nonequal,
        const bool is_firstround);

    unsigned int create_set(pFunc mf, const vector <string> & equal_indice_names,
        const vector<const StringManipulator*>& pmanipulators_equal,
        const vector <string> & nonequal_indice_names,
        const vector<const StringManipulator*>& pmanipulators_nonequal );

    void print (std::ostream & os, const string & unique_record_id_name ) const;

    void reset(const unsigned int num_cols);

};


class cWorker_For_Training : public Thread {

private:

  map<string, RecordPList> ::iterator *piter;

  cBlocking_For_Training::pFunc func;

  const vector < string > & m_equal_indice_names;

  const vector < const StringManipulator * > & m_pstringcontrol_equal;

  const vector < string > & m_non_equal_indice_names;

  const vector < const StringManipulator * > & m_pstringcontrol_nonequal;

  static pthread_mutex_t iter_mutex;

 public:

  explicit cWorker_For_Training(map < string, RecordPList>::iterator *inputiter,
      const cBlocking_For_Training::pFunc inputfun,
      const vector < string > & equal_indice_names,
      const vector < const StringManipulator * > & pmanipulators_equal,
      const vector < string > & nonequal_indice_names,
      const vector < const StringManipulator * > & pmanipulators_nonequal)
    : piter(inputiter), func(inputfun), m_equal_indice_names(equal_indice_names),
    m_pstringcontrol_equal(pmanipulators_equal),
    m_non_equal_indice_names(nonequal_indice_names),
    m_pstringcontrol_nonequal(pmanipulators_nonequal) {};

  ~cWorker_For_Training() {};

  void run();
};


bool   make_stable_training_sets_by_personal    (const list <Record> & all_records,
                                                 const unsigned int limit,
                                                 const vector <string> & training_filenames);



void         find_rare_names_v2 (const vector < RecordPList * > &vec_pdest,
                                 const list< const Record* > & source);

unsigned int create_tset02      (list <RecordPairs> &results,
                                 const RecordPList & reclist,
                                 const vector <string> & column_names,
                                 const vector < const RecordPList * > & vec_prare_names,
                                 const unsigned int limit );

/**
 * @todo Write some documentation
 */
unsigned int create_xset03      (list <RecordPairs> &results,
                                 //const list <const Record*> & reclist,
                                 const vector < const RecordPList * > & vec_prare_names,
                                 const unsigned int limit );

#endif /* PATENT_TRAINING_H */
