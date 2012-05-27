/*
 * DisambigEngine.h
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
 */

#ifndef PATENT_RECORD_H
#define PATENT_RECORD_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>


using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;


/*
 * definition of Record class.
 * The Record class is used to save a real line of record which includes multiple concrete attributes.
 *
 *
 * Private:
 *         vector <const Attribute *> vector_pdata: the major data of the class, which stores a vector of pointers of concrete attributes.
 *         static vector <string> column_names: static member which stores the name of each attributes with sequential information. Very important.
 *         static vector < string > active_similarity_names: static member which stores the names of attributes whose comparison functions are activated.
 *         static const Record * sample_record_pointer: a pointer of a real record object, allowing some polymorphic static functions.
 *
 */

/*
 * Public:
 *  Record(const vector <const Attribute *>& input_vec): vector_pdata(input_vec): constructor.
 *  Record(): default constructor.
 *  vector <unsigned int> record_compare(const Record & rhs) const: compare (*this) with rhs and get a similarity profile.
 *           NOTE THAT SIMILARITY PROFILES ARE vector <unsigned int>s;
 *  Example: if (Firstname, Lastname, Assignee, class, Coauthor) are activated for comparison, the function will return a
 *           vector < unsigned int > of 5-dimensions, each of which indicating a score for its corresponding column.
 *  vector <unsigned int> record_compare_by_attrib_indice (const Record &rhs, const vector < unsigned int > & attrib_indice_to_compare) const:
 *    compare (*this) with rhs only in the columns whose indice are given in "attrib_indice_to_compare", and returns an incomplete similarity profile.
 *  Example: if (Firstname, Lastname, Assignee, class, Coauthor) are activated, and the attrib_indice_to_compare = [ 0, 2, 3],
 *  then the return value is a vector < unsigned int > = [ Firstname, Assignee, Class];
 */


/*
 *        static unsigned int get_index_by_name(const string & inputstr): get the index of the attribute whose specifier is "inputstr"
 *        const vector <const string * > & get_data_by_index(const unsigned int i) const: get the REAL data of the ith attribute through its index.
 *        const Attribute * const & get_attrib_pointer_by_index(const unsigned int i) const:
 *                get the reference of the const pointer to a const attribute based on its index. Returning of reference is critical,
 *                because this allows modification of the attribute pointer in the vector.
 *        const vector < const Attribute*> & get_attrib_vector () const: returns the vector of attribute pointers.
 *        void print( std::ostream & os ) const: output the record to an output stream, like std::cout or ofstream.
 *        void print() const: output the record to std::cout. Used mainly for debug purpose.
 *        static const vector < string > & get_column_names(): get the attribute names vector.
 *        unsigned int informative_attributes() const: count the number of informative attributes ( those containing "something" rather than empty strings ).
 *        unsigned int record_exact_compare(const Record & rhs ) const: compare (*this) with rhs and return the number of attributes that are exactly the same.
 *        void set_attrib_pointer_by_index( const Attribute * pa, const unsigned int i ): modify the attribute vector, setting the ith element to pa
 *        static void clean_member_attrib_pool(): clear all the members static attribute pool. This is EXTREMELY dangerous, unless its functionality is fully understood.
 *        static unsigned int record_size(): returns the number of attributes of a record.
 *        static void update_active_similarity_names(): update the static member "active_similarity_names" by checking the comparator of each attribute.
 *        static const vector < string > & get_similarity_names(): get the names of the active similarity profile attributes.
 *        static unsigned int get_similarity_index_by_name(const string & inputstr): get the index of an attribute in the ACTIVE similarity profile.
 *        static const Record & get_sample_record(): return the sample record object for external use.
 *
 */
class Record {

    friend bool fetch_records_from_txt(list <Record> & source, const char * txt_file, const vector<string> &requested_columns);
    friend void clear_records(const list <Record> & source);
    friend class cSort_by_attrib;
    friend class cRatioComponent;

private:
    vector <const Attribute *> vector_pdata;
    static vector <string> column_names;
    static vector < string > active_similarity_names;
    static const Record * sample_record_pointer;

public:
    Record(const vector <const Attribute *>& input_vec): vector_pdata(input_vec) {};
    Record() {}
    vector <unsigned int> record_compare(const Record & rhs) const;
    vector <unsigned int> record_compare_by_attrib_indice (const Record &rhs, const vector < unsigned int > & attrib_indice_to_compare) const;
    static unsigned int get_index_by_name(const string & inputstr);
    const vector <const string * > & get_data_by_index(const unsigned int i) const {return vector_pdata.at(i)->get_data();};
    const Attribute * const & get_attrib_pointer_by_index(const unsigned int i) const {return vector_pdata.at(i);} //return a reference is very important, because the content can be changed.
    const vector < const Attribute*> & get_attrib_vector () const { return vector_pdata;}
    void print( std::ostream & os ) const;

    void print() const; //{ this->print(std::cout);}
    static const vector < string > & get_column_names() { return column_names;}
    unsigned int informative_attributes() const;
    unsigned int record_exact_compare(const Record & rhs ) const;
    void set_attrib_pointer_by_index( const Attribute * pa, const unsigned int i ) { vector_pdata.at(i) = pa;}
    static void clean_member_attrib_pool();
    static unsigned int record_size() { return column_names.size();}
    static void update_active_similarity_names();
    static const vector < string > & get_similarity_names() { return active_similarity_names;}
    static unsigned int get_similarity_index_by_name(const string & inputstr);
    static const Record & get_sample_record() { return *sample_record_pointer;}
    void reconfigure_record_for_interactives() const;
    static void activate_comparators_by_name ( const vector < string > &);
};

#endif /* PATENT_RECORD_H */
