
#ifndef PATENT_RECORD_H
#define PATENT_RECORD_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

// TODO: Move this into a header all other headers include:
#include <stdint.h>

#include "typedefs.h"
#include "attribute.h"
#include "threading.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;



/**
 * The Record class is used to save a real line of record which
 * includes multiple concrete attributes.
 */
class Record {

    friend bool fetch_records_from_txt(list <Record> & source,
        const char * txt_file, const vector<string> &requested_columns);

    friend void clear_records(const list <Record> & source);

    friend class cSort_by_attrib;

    friend class cRatioComponent;


private:

   /**
    * vector <const Attribute *> vector_pdata: the major data of the class,
    * which stores a vector of pointers of concrete attributes.
    */
    // TODO: s/vector_pdata/attributes/g
    vector <const Attribute *> vector_pdata;

   /**
    * static vector <string> column_names: static member which stores the
    * name of each attributes with sequential information. Very important.
    */
    static vector <string> column_names;

   /**
    * static vector < string > active_similarity_names:
    * static member which stores the names of attributes
    * whose comparison functions are activated.
    */
    static vector < string > active_similarity_names;

   /**
    * static const Record * sample_record_pointer: a pointer of a real
    * record object, allowing some polymorphic static functions.
    */
    static const Record * sample_record_pointer;

public:

   /**
    * Public:
    *  Record(const vector <const Attribute *>& input_vec):
    *  vector_pdata(input_vec): constructor.
    */
    Record(const vector <const Attribute *> & input_vec)
           : vector_pdata(input_vec) {};

    // Record(): default constructor.
    Record() {}


    void set_column_names(std::vector<std::string> cn) {
      column_names = cn;
    }

   /**
    *  vector <uint32_t> record_compare(const Record & rhs) const:
    *  compare (*this) with rhs and get a similarity profile.
    *  NOTE THAT SIMILARITY PROFILES ARE vector <uint32_t>s;
    *
    *  Example: if (Firstname, Lastname, Assignee, class, Coauthor) are
    *  activated for comparison, the function will return a
    *  vector < uint32_t > of 5-dimensions, each of which
    *  indicating a score for its corresponding column.
    */
    //vector <uint32_t> record_compare(const Record & rhs) const;
    SimilarityProfile record_compare(const Record & rhs) const;

   /**
    *  vector <uint32_t> record_compare_by_attrib_indice (const Record &rhs,
    *  const vector < uint32_t > & attrib_indice_to_compare) const:
    *
    *  compare (*this) with rhs only in the columns whose indice are given
    *  in "attrib_indice_to_compare", and returns an incomplete similarity profile.
    *  Example: if (Firstname, Lastname, Assignee, class, Coauthor) are activated,
    *  and the attrib_indice_to_compare = [ 0, 2, 3],
    *  then the return value is a vector < uint32_t > = [ Firstname, Assignee, Class];
    */
    //vector <uint32_t> record_compare_by_attrib_indice (const Record &rhs,
    SimilarityProfile record_compare_by_attrib_indice (const Record &rhs,
        const vector<uint32_t> & attrib_indice_to_compare) const;


   /**
    * get the index of the attribute whose specifier is "inputstr"
    */
    static uint32_t get_index_by_name(const string & inputstr);

   /**
    * uint32_t informative_attributes() const:
    * count the number of informative attributes (those containing
    * "something" rather than empty strings).
    */
    uint32_t informative_attributes() const;

   /**
    * const vector <const string * > & get_data_by_index(const uint32_t i) const:
    * get the REAL data of the ith attribute through its index.
    *
    * (As opposed to the "FAKE" data...?)
    */
    const vector <const string *> & get_data_by_index(const uint32_t i) const {
      // vector_pdata is vector<Attribute *>
      // The get_data() function returns, as noted in this function's definition,
      // a vector<const string *>, but I'm not sure what these strings are.
      // Probably, they are simply the attribute names. It's all so horribly meta.
      // TODO: try printing this out in the record unit test.
      return vector_pdata.at(i)->get_data();
    };


   /**
    * const Attribute * const & get_attrib_pointer_by_index(const uint32_t i) const:
    *  get the reference of the const pointer to a const attribute based on its index.
    *  Returning of reference is critical,
    *  because this allows modification of the attribute pointer in the vector.
    */
    //return a reference is very important, because the content can be changed.
    const Attribute * const & get_attrib_pointer_by_index(const uint32_t i) const {
      return vector_pdata.at(i);
    }


    const vector<const Attribute*> & get_attrib_vector () const {
      return vector_pdata;
    }


    void print(std::ostream & os) const;

    void print() const;

    void print_sample_record();

    static const vector<string> & get_column_names() {
      return column_names;
    }


   /**
    * uint32_t record_exact_compare(const Record & rhs ) const:
    * compare (*this) with rhs and return the number of attributes that are exactly the same.
    */
    uint32_t record_exact_compare(const Record & rhs ) const;

   /**
    * void set_attrib_pointer_by_index( const Attribute * pa, const uint32_t i ):
    * modify the attribute vector, setting the ith element to pa
    */
    void set_attrib_pointer_by_index(const Attribute * pa, const uint32_t i) {
      vector_pdata.at(i) = pa;
    }


   /**
    * static void clean_member_attrib_pool():
    * clear all the members static attribute pool.
    * This is EXTREMELY dangerous, unless its functionality is fully understood.
    */
    static void clean_member_attrib_pool();


   /**
    * static uint32_t record_size():
    * returns the number of attributes of a record.
    *
    * TODO: This is only used in two places in the clustering
    * code, it can probably be (carefully) removed. Also, the
    * name needs to be changed to get_number_of_attributes();
    */
    static uint32_t record_size() {
      return column_names.size();
    }


   /**
    * static void update_active_similarity_names():
    * update the static member "active_similarity_names"
    * by checking the comparator of each attribute.
    */
    static void update_active_similarity_names();


   /**
    * static const vector < string > & get_similarity_names():
    * get the names of the active similarity profile attributes.
    */
    static const vector<string> & get_similarity_names() {
      return active_similarity_names;
    }


   /**
    * static uint32_t get_similarity_index_by_name(const string & inputstr):
    * get the index of an attribute in the ACTIVE similarity profile.
    */
    static uint32_t get_similarity_index_by_name(const string & inputstr);


   /**
    * static const Record & get_sample_record():
    * return the sample record object for external use.
    */
    static const Record & get_sample_record() {
      return *sample_record_pointer;
    }

    static void set_sample_record(Record * r) {
      sample_record_pointer = r;
    }

    void reconfigure_record_for_interactives() const;

    static void activate_comparators_by_name (const vector<string> &);
};

#endif /* PATENT_RECORD_H */
