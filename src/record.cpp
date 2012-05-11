/*
 * DisambigEngine.cpp
 *
 * Created on: Dec 13, 2010
 * Author: ysun
 */

#include "DisambigEngine.h"
#include "DisambigDefs.h"
#include "DisambigFileOper.h"
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
 * Declaration ( and definition ) of static members in some classes.
 */
vector <string> cRecord::column_names;
vector <string> cRecord::active_similarity_names;
const cRecord * cRecord::sample_record_pointer = NULL;

//const string cBlocking_Operation::delim = "##";


/*
 * Aim: to check the number of columns that are supposed to be useful in a cRecord object.
 * 		Firstname, middlename, lastname, assignee (company), latitude and city are believed to be useful.
 * 		Other attributes, such as street and coauthor, are allowed to be missing.
 * Algorithm: use " is_informative() " function to check each specified attribute, and return the sum.
 */

unsigned int 
cRecord::informative_attributes() const {

	static const unsigned int firstname_index = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	static const unsigned int middlename_index = cRecord::get_index_by_name(cMiddlename::static_get_class_name());
	static const unsigned int lastname_index = cRecord::get_index_by_name(cLastname::static_get_class_name());
	static const unsigned int assignee_index = cRecord::get_index_by_name(cAssignee::static_get_class_name());
	static const unsigned int lat_index = cRecord::get_index_by_name(cLatitude::static_get_class_name());
	static const unsigned int ctry_index = cRecord::get_index_by_name(cCountry::static_get_class_name());

	unsigned int cnt = 0;

	this->vector_pdata.at(firstname_index)->is_informative() && (++cnt);
	this->vector_pdata.at(middlename_index)->is_informative() && (++cnt);
	this->vector_pdata.at(lastname_index)->is_informative() && (++cnt);
	this->vector_pdata.at(assignee_index)->is_informative() && (++cnt);
	this->vector_pdata.at(lat_index)->is_informative() && (++cnt);
	this->vector_pdata.at(ctry_index)->is_informative() && (++cnt);

	return cnt;
}

/*
 * Aim: to keep updated the names of current similarity profile columns.
 * Algorithm: use a static sample cRecord pointer to check the comparator status of each attribute.
 * 				Clears the original cRecord::active_similarity_names and update with a newer one.
 */
void 
cRecord::update_active_similarity_names() {

	cRecord::active_similarity_names.clear();
	const cRecord * pr = cRecord::sample_record_pointer;
	for ( vector < const cAttribute *>::const_iterator p = pr->vector_pdata.begin(); p != pr->vector_pdata.end(); ++p ) {
		//std::cout << (*p)->get_class_name() << " , ";		//for debug purpose
		if ( (*p)->is_comparator_activated() )
			cRecord::active_similarity_names.push_back((*p)->get_class_name());
	}
}

/*
 * Aim: a global function that performs the same functionality
 * as the above one. However, this function is declared and callable in
 * the template implementations in "DisambigDefs.h", where cRecord has 
 * not be declared yet.
 */
void 
cRecord_update_active_similarity_names() {

  cRecord::update_active_similarity_names();
}


/*
 * Aim: to print a record to an ostream object, such as a file 
 * stream or a standard output (std::cout). Algorithm: call 
 * each attribute pointer's "print( ostream )" method in the record object.
 */
void 
cRecord::print(std::ostream & os ) const {

  const char lend = '\n';
  for ( vector <const cAttribute *>::const_iterator p = this->vector_pdata.begin(); p != this->vector_pdata.end(); ++p )
	(*p)->print( os );
  os << "===============================" << lend;
}


/*
 * Aim: compare (*this) record object with rhs record object, and 
 * return a similarity profile ( which is vector < unsigned int > ) 
 * for all activated columns.
 * Algorithm: call each attribute pointer's "compare" method.
 */

vector <unsigned int> 
cRecord::record_compare(const cRecord & rhs) const {

	static const bool detail_debug = false;
	vector <unsigned int > rec_comp_result;
	if ( detail_debug ) {
		static const unsigned int uid_index = cRecord::get_index_by_name(cUnique_Record_ID::static_get_class_name());
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
	try{
		for ( unsigned int i = 0; i < this->vector_pdata.size(); ++i ) {
			try {
				unsigned int stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
				rec_comp_result.push_back( stage_result );
			}
			catch (const cException_No_Comparision_Function & err) {
				//std::cout << err.what() << " does not have comparision function. " << std::endl; //for debug purpose
			}
		}
	}
	catch ( const cException_Interactive_Misalignment & except) {
		std::cout << "Skipped" << std::endl;
		rec_comp_result.clear();
	}

	//for debug only.
	if ( detail_debug ) {
		static const unsigned int uid_index = cRecord::get_index_by_name(cUnique_Record_ID::static_get_class_name());
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
			for ( vector < unsigned int >::const_iterator t = rec_comp_result.begin(); t != rec_comp_result.end(); ++t )
				std::cout << *t << ",";
			std::cout << std::endl << std::endl;
		}
	}


	return rec_comp_result;
}



/*
 * Aim: compare (*this) record object with rhs record object, and returns a similarity profile for columns that
 * are both activated and passed in the "attrib_indice_to_compare" vector.
 * Algorithm: call each attribute pointer's "compare" method.
 *
 */
vector <unsigned int> 
cRecord::record_compare_by_attrib_indice (const cRecord &rhs, 
                                          const vector < unsigned int > & attrib_indice_to_compare) const {

	vector <unsigned int > rec_comp_result;
	try{
		for ( unsigned int j = 0; j < attrib_indice_to_compare.size(); ++j ) {
			try {
				unsigned int i = attrib_indice_to_compare.at(j);
				unsigned int stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
				rec_comp_result.push_back( stage_result );
			}
			catch (const cException_No_Comparision_Function & err) {
				//std::cout << err.what() << " does not have comparision function. " << std::endl;
			}
		}
	}
	catch ( const cException_Interactive_Misalignment & except) {
		std::cout << "Skipped" << std::endl;
		rec_comp_result.clear();
	}
	return rec_comp_result;
}


/*
 * Aim: to check the number of exact identical attributes between (*this) and rhs record objects.
 * Algorithm: call each attribute pointer's "exact_compare" method.
 */
unsigned int 
cRecord::record_exact_compare(const cRecord & rhs ) const {

	unsigned int result = 0;
	for ( unsigned int i = 0; i < this->vector_pdata.size(); ++i ) {
		int ans = this->vector_pdata.at(i)->exact_compare( * rhs.vector_pdata.at(i));
		if ( 1 == ans )
			++result;
	}
	return result;
}


/*
 * Aim: print the record on standard output. The definition here is to avoid inlineness to allow debugging.
 */
void 
cRecord::print() const { 

  this->print(std::cout);
}


/*
 * Aim: to clean some specific attribute pool.
 * Algorithm: for those whose reference counting = 0, this operation will delete those nodes.
 *
 */
void 
cRecord::clean_member_attrib_pool() {

	for ( vector < const cAttribute *>::const_iterator p = sample_record_pointer->vector_pdata.begin();
			p != sample_record_pointer->vector_pdata.end(); ++p )
		(*p)->clean_attrib_pool();
}


/*
 * Aim: get the index of the desired column name in the columns read from text file.
 * Algorithm: exhaustive comparison. Time complexity = O(n); if no matching is found, a exception will be thrown.
 */
unsigned int 
cRecord::get_index_by_name(const string & inputstr) {

	for ( unsigned int i = 0 ; i < column_names.size(); ++i ) {
		if ( column_names.at(i) == inputstr ) {
			return i;
                }
        }

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
        string s = string("from ") + string(__FILE__) + string(":") 
                 + string(STRINGIZE(__LINE__)) + string("column name: ");
        s += inputstr;
        //string s = inputstr + string("from "__FILE__":"__LINE__);
	//throw cException_ColumnName_Not_Found(inputstr.c_str());
	throw cException_ColumnName_Not_Found(s.c_str());
}


/**
 * Aim: get the index of the desired column name in the 
 * active similarity profile columns.
 * 
 * Algorithm: exhaustive comparison. Time complexity = O(n); 
 * if no matching is found, a exception will be thrown.
 */
unsigned int 
cRecord::get_similarity_index_by_name(const string & inputstr) {

	for ( unsigned int i = 0 ; i < active_similarity_names.size(); ++i )
		if ( active_similarity_names.at(i) == inputstr )
			return i;
	throw cException_ColumnName_Not_Found(inputstr.c_str());
}


void
cRecord::activate_comparators_by_name (const vector < string > & inputvec) {

    cRecord::active_similarity_names = inputvec;

    for ( vector < const cAttribute *>::const_iterator p = cRecord::sample_record_pointer->get_attrib_vector().begin();
            p != cRecord::sample_record_pointer->get_attrib_vector().end(); ++p ) {

        const string & classlabel = (*p)->get_class_name();

        if (std::find( inputvec.begin(), inputvec.end(), classlabel) == inputvec.end()) {
            (*p)->deactivate_comparator();
        }
        else {
            (*p)->activate_comparator();
        }
    }

    cRecord::update_active_similarity_names();
}
