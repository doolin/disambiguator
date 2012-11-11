
#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <numeric>

#include "record.h"
#include "engine.h"
#include "attribute.h"
#include "cluster.h"
#include "ratios.h"
#include "newcluster.h"

using std::map;
using std::set;

/**
 * Declaration (and definition) of static members.
 */
vector <string> Record::column_names;
vector <string> Record::active_similarity_names;
const Record * Record::sample_record_pointer = NULL;

//const string cBlocking_Operation::delim = "##";


/**
 * Aim: to check the number of columns that are
 * supposed to be useful in a Record object.
 *
 * Firstname, middlename, lastname, assignee (company),
 * latitude and city are believed to be useful.
 *
 * Other attributes, such as street and coauthor,
 * are allowed to be missing.
 *
 * Algorithm: use " is_informative() " function to
 * check each specified attribute, and return the sum.
 */
uint32_t
Record::informative_attributes() const {

    static const uint32_t firstname_index = Record::get_index_by_name(cFirstname::static_get_class_name());
    static const uint32_t middlename_index = Record::get_index_by_name(cMiddlename::static_get_class_name());
    static const uint32_t lastname_index = Record::get_index_by_name(cLastname::static_get_class_name());
    static const uint32_t assignee_index = Record::get_index_by_name(cAssignee::static_get_class_name());
    static const uint32_t lat_index = Record::get_index_by_name(cLatitude::static_get_class_name());
    static const uint32_t ctry_index = Record::get_index_by_name(cCountry::static_get_class_name());

    uint32_t cnt = 0;

    this->vector_pdata.at(firstname_index)->is_informative() && (++cnt);
    this->vector_pdata.at(middlename_index)->is_informative() && (++cnt);
    this->vector_pdata.at(lastname_index)->is_informative() && (++cnt);
    this->vector_pdata.at(assignee_index)->is_informative() && (++cnt);
    this->vector_pdata.at(lat_index)->is_informative() && (++cnt);
    this->vector_pdata.at(ctry_index)->is_informative() && (++cnt);

    return cnt;
}


void
Record::print_sample_record() {
  const Record * pr = Record::sample_record_pointer;
  pr->print();
}


/**
 * Aim: to keep updated the names of current similarity profile columns.
 * Algorithm: use a static sample Record pointer to check the comparator status of each attribute.
 *                 Clears the original Record::active_similarity_names and update with a newer one.
 */
void
Record::update_active_similarity_names() {

    Record::active_similarity_names.clear();
    const Record * pr = Record::sample_record_pointer;

    vector<const Attribute *>::const_iterator p = pr->vector_pdata.begin();
    for (; p != pr->vector_pdata.end(); ++p) {
        //std::cout << (*p)->get_class_name() << " , "; //for debug purpose

        if ((*p)->is_comparator_activated()) {
            Record::active_similarity_names.push_back((*p)->get_class_name());
        }
    }
}


/**
 * Aim: a global function that performs the same functionality
 * as the above one. However, this function is declared and callable in
 * the template implementations in "DisambigDefs.h", where Record has 
 * not be declared yet.
 *
 * This is a really bad idea, and just the action of having to expose
 * a class method to global access exposes a fundamentally flawed design.
 */
void
Record_update_active_similarity_names() {

  Record::update_active_similarity_names();
}


void
Record::print(std::ostream & os) const {

  vector <const Attribute *>::const_iterator a = this->vector_pdata.begin();
  for (a; a != this->vector_pdata.end(); ++a) {
    (*a)->print(os );
  }

  os << "===============================" << "\n";
}


void
Record::print() const {

  this->print(std::cout);
}


/**
 * Aim: compare (*this) record object with rhs record object, and
 * return a similarity profile (which is vector<uint32_t>)
 * for all activated columns.
 *
 * Algorithm: call each attribute pointer's "compare" method.
 */
SimilarityProfile
Record::record_compare(const Record & rhs) const {

    static const bool detail_debug = false;
    SimilarityProfile sp;

    /////////////////////////////////////
    // TODO: Refactor this debugging code
    if ( detail_debug ) {

        static const uint32_t uid_index = Record::get_index_by_name(cUnique_Record_ID::static_get_class_name());
        const string debug_string = "06476708-1";
        const string * ps = this->get_attrib_pointer_by_index(uid_index)->get_data().at(0);
        const string * qs = rhs.get_attrib_pointer_by_index(uid_index)->get_data().at(0);

        if ( *ps == debug_string || * qs == debug_string ) {
            std::cout << "Before record compare: "<< std::endl;
            std::cout << "-----------" << std::endl;
            this->print();
            std::cout << "===========" << std::endl;
            rhs.print();
            std::cout << std::endl << std::endl;
        }
    }
    /////////////////////////////// End refactor /////////////////

    ///////////////////////// /////////////////
    // TODO: Refactor this block as it's duplicated
    // with record_compare_attrib_indice
    try {

        for (uint32_t i = 0; i < this->vector_pdata.size(); ++i) {
            try {
                uint32_t stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
                sp.push_back(stage_result);
            }
            catch (const cException_No_Comparision_Function & err) {
                //std::cout << err.what() << " does not have comparision function. " << std::endl; //for debug purpose
            }
        }
    } catch (const cException_Interactive_Misalignment & except) {

        std::cout << "Skipped" << std::endl;
        sp.clear();
    }
    /////////////////// End refactor /////////////////

    // //////////////////////////////////////////////
    // TODO: Refactor to it's own function
    if (detail_debug) {
        static const uint32_t uid_index = Record::get_index_by_name(cUnique_Record_ID::static_get_class_name());
        const string debug_string = "06476708-1";
        const string * ps = this->get_attrib_pointer_by_index(uid_index)->get_data().at(0);
        const string * qs = rhs.get_attrib_pointer_by_index(uid_index)->get_data().at(0);

        if ( *ps == debug_string || * qs == debug_string ) {
            std::cout << "After record compare: "<< std::endl;
            std::cout << "-----------" << std::endl;
            this->print();
            std::cout << "===========" << std::endl;
            rhs.print();
            std::cout << "..........." << std::endl;
            std::cout << "Similarity Profile =";

            for ( vector < uint32_t >::const_iterator t = sp.begin(); t != sp.end(); ++t )
                std::cout << *t << ",";
            std::cout << std::endl << std::endl;
        }
    }
    /////////////////// End refactor /////////////////

    return sp;
}


/**
 * Aim: compare (*this) record object with rhs record object,
 * and returns a similarity profile for columns that
 * are both activated and passed in the "attrib_indice_to_compare" vector.
 * Algorithm: call each attribute pointer's "compare" method.
 */
//vector <uint32_t>
SimilarityProfile
Record::record_compare_by_attrib_indice (const Record &rhs,
                                         const vector < uint32_t > & attrib_indice_to_compare) const {

    SimilarityProfile sp;

    if (0 == attrib_indice_to_compare.size()) {
      std::cout << "Attrib index is empty" << std::endl;
      exit(0);
    }

    try {

        for ( uint32_t j = 0; j < attrib_indice_to_compare.size(); ++j ) {

            try {
                uint32_t i = attrib_indice_to_compare.at(j);
                uint32_t stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
                //std::cout << "stage_result: " << stage_result << std::endl;
                sp.push_back(stage_result);
            }
            catch (const cException_No_Comparision_Function & err) {
                //std::cout << err.what() << " does not have comparision function. " << std::endl;
            }
        }
    } catch ( const cException_Interactive_Misalignment & except) {

        std::cout << "Skipped" << std::endl;
        sp.clear();
    }

    return sp;
}


/**
 * Check the number of exact identical attributes between
 * (*this) and rhs record objects. Algorithm: call each
 * attribute pointer's "exact_compare" method.
 */
uint32_t
Record::record_exact_compare(const Record & rhs ) const {

    uint32_t result = 0;

    for (uint32_t i = 0; i < this->vector_pdata.size(); ++i) {
        int ans = this->vector_pdata.at(i)->exact_compare(*rhs.vector_pdata.at(i));

        if ( 1 == ans ) ++result;
    }

    return result;
}



/*
 * Clean some specific attribute pool.
 * Algorithm: for those whose reference counting = 0,
 * this operation will delete those nodes.
 */
void
Record::clean_member_attrib_pool() {

    vector < const Attribute *>::const_iterator p = sample_record_pointer->vector_pdata.begin();
    for (; p != sample_record_pointer->vector_pdata.end(); ++p) {
        (*p)->clean_attrib_pool();
    }
}


/**
 * Get the index of the desired column name in the columns read from text file.
 * Algorithm: exhaustive comparison. Time complexity = O(n);
 * if no matching is found, a exception will be thrown.
 */
uint32_t
Record::get_index_by_name(const string & inputstr) {

    for (uint32_t i = 0 ; i < column_names.size(); ++i) {
        if (column_names.at(i) == inputstr) {
            return i;
        }
    }

    string s = string("from ") + string(__FILE__) + string(":")
             + string(STRINGIZE(__LINE__)) + string(", column name: ");
    s += inputstr;
    throw cException_ColumnName_Not_Found(s.c_str());
}


/**
 * Aim: get the index of the desired column name in the
 * active similarity profile columns.
 *
 * Algorithm: exhaustive comparison. Time complexity = O(n);
 * if no matching is found, a exception will be thrown.
 */
uint32_t
Record::get_similarity_index_by_name(const string & inputstr) {

    for (uint32_t i = 0 ; i < active_similarity_names.size(); ++i)
        if (active_similarity_names.at(i) == inputstr)
            return i;

    throw cException_ColumnName_Not_Found(inputstr.c_str());
}


bool
cant_find_label(const vector<string> & labels, const string & classlabel) {

  if (std::find(labels.begin(), labels.end(), classlabel) == labels.end())
    return true;
  return false;
}


void
Record::activate_comparators_by_name (const vector<string> & inputvec) {

    Record::active_similarity_names = inputvec;

    vector<const Attribute *>::const_iterator p = Record::sample_record_pointer->get_attrib_vector().begin();
    for (; p != Record::sample_record_pointer->get_attrib_vector().end(); ++p) {

        const string & classlabel = (*p)->get_class_name();

        if (std::find(inputvec.begin(), inputvec.end(), classlabel) == inputvec.end()) {
        //if (cant_find_label(inputvec, classlabel)) {
            (*p)->deactivate_comparator();
        }
        else {
            (*p)->activate_comparator();
        }
    }

    Record::update_active_similarity_names();
}


void
Record::reconfigure_record_for_interactives() const {

    vector <const Attribute *>::const_iterator cipa = vector_pdata.begin();
    for (; cipa != vector_pdata.end(); ++cipa) {
        (*cipa)->reconfigure_for_interactives(this);
    }
}

