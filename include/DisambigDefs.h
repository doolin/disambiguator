/*
 * DisambigDefs.h
 *
 *  Created on: Dec 7, 2010
 *  Author: Ye (Edward) Sun.  Harvard University. sunyedinken2002@hotmail.com
 *
 *  This file contains most of the basic definitions.
 */


#ifndef DISAMBIGLIB_HPP_
#define DISAMBIGLIB_HPP_

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <pthread.h>
#include <typeinfo>
#include <memory>

#define INERT_ATTRIB_GROUP_IDENTIFIER "NONE" // the attribute group specifier that is not the component of similarity profiles


using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

class cRecord;
class cRecord_Reconfigurator;
class Attribute;

void cRecord_update_active_similarity_names();
const cRecord_Reconfigurator * generate_interactive_reconfigurator( const Attribute * pAttrib);
void reconfigure_interactives ( const cRecord_Reconfigurator * pc, const cRecord * pRec);


#include "exceptions.h"
#include "attribute.h"
#include "DisambigComp.h"
//#include "DisambigCustomizedDefs.h"


/*
 * DisambigCustomizedDefs.h
 *
 *  Created on: Apr 5, 2011
 *      Author: ysun
 */

//#ifndef DISAMBIGCUSTOMIZEDDEFS_H_
//#define DISAMBIGCUSTOMIZEDDEFS_H_

/*
 * For each concrete class, if it can be one of the components in the similarity profile, whether its comparator is activated or not,
 * a "STATIC CONST UNSIGNED INT" member, namely max_value, should be declared and defined in the concrete class. At the same time,
 * the virtual function get_attrib_max_value() const has to be overridden, with comparator activity check inside.
 *
 * Each concrete class should also provide a default constructor ( which is actually very simple ).
 */


/*
 * IMPORTANT:
 * FOR EACH CONCRETE CLASS, IF ITS STATIC MEMBER IS DIFFERENT FROM DEFAULT, A TEMPLATE SPECIALIZATION MUST BE DECLARED BEFORE THE DEFAULT IS
 * DECLARED AND DEFINED. AFTER THAT, THE SPECIALIZED DEFINITION SHOULD APPEAR IN THE CPP FILE.
 * For example, the attrib group of cFirstname is "Personal" instead of the default "NONE". In this case, the specialized declaration must be made
 * before the general default declaration. i.e.
 * template <> const string Attribute_Intermediary<cFirstname>::attrib_group;
 * appears before:
 * template <typename Derived> const string Attribute_Intermediary<Derived>::attrib_group = INERT_ATTRIB_GROUP_IDENTIFIER;
 * And then in the cpp file, there is:
 * template <> const string Attribute_Intermediary<cFirstname>::attrib_group = "Personal";
 *
 *
 * See more information and example in the end of this file.
 */


class cFirstname : public Attribute_Single_Mode <cFirstname> {
private:
  static unsigned int previous_truncation;
  static unsigned int current_truncation;
 public:
  static void set_truncation( const unsigned int prev, const unsigned int cur ) { previous_truncation = prev; current_truncation = cur; }
	//static const unsigned int max_value = Jaro_Wrinkler_Max;
	static const unsigned int max_value = 4;

	cFirstname(const char * source = NULL) {}
	bool split_string(const char*);		//override because some class-specific splitting is involved.
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	//override the base class to enable the functionality of the function.
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
	unsigned int compare(const Attribute & right_hand_side) const ;
};


class cLastname : public Attribute_Single_Mode <cLastname> {
public:
	static const unsigned int max_value = Jaro_Wrinkler_Max;

	cLastname(const char * source = NULL ) {}
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
};

class cMiddlename : public Attribute_Single_Mode <cMiddlename> {
public:
	static const unsigned int max_value = 3;

	cMiddlename(const char * source = NULL ) {}
	unsigned int compare(const Attribute & rhs) const;		//override to allow customization.
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
};

class cLatitude_Data : public Attribute_Single_Mode<cLatitude_Data> {};

class cLatitude : public Attribute_Interactive_Mode <cLatitude, cLatitude_Data> {
private:
	static const unsigned int max_value = 5;
public:
	cLatitude(const char * source = NULL ) {}
	unsigned int compare(const Attribute & rhs) const;	//override to customize
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
};

class cLongitude_Data : public Attribute_Single_Mode<cLongitude_Data> {};

class cLongitude: public Attribute_Interactive_Mode <cLongitude, cLongitude_Data > {
private:
	static const unsigned int max_value = 1;
public:
	cLongitude(const char * source = NULL ) {}
	unsigned int compare(const Attribute & rhs) const;	//override to customize
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
};

class cStreet: public Attribute_Single_Mode <cStreet> {
public:
	cStreet(const char * source = NULL) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE Street IS NOT BEING COMPARED either. IT IS WITH THE LATITUDE COMPARISION.

};

// Modeled after the Street class
class cState: public Attribute_Single_Mode <cState> {
public:
	cState(const char * source = NULL) {}
};

// Modeled after the Street class
class cZipcode: public Attribute_Single_Mode <cZipcode> {
public:
	cZipcode(const char * source = NULL) {}
};

// Modeled after the Street class
/*
class cAppDateStr: public Attribute_Single_Mode <cAppDateStr> {
public:
	cAppDateStr(const char * source = NULL) {}
};
*/

class cAppDate: public Attribute_Single_Mode <cAppDate> {
public:
	cAppDate(const char * source = NULL) {}
};

// Modeled after the Street class
class cInvSeq: public Attribute_Single_Mode <cInvSeq> {
public:
	cInvSeq(const char * source = NULL) {}
};

// Modeled after the Street class
class cinvnum_N: public Attribute_Single_Mode <cinvnum_N> {
public:
	cinvnum_N(const char * source = NULL) {}
};

class cinvnum: public Attribute_Single_Mode <cinvnum> {
public:
	cinvnum(const char * source = NULL) {}
};

class cCountry: public Attribute_Single_Mode <cCountry> {
public:
	static unsigned int const max_value = 2;
	cCountry(const char * source = NULL ) {}
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
	unsigned int compare(const Attribute & right_hand_side) const;

};


// cClass and cCoauthor are in set_mode, not single_mode
class cClass: public Attribute_Set_Mode < cClass > {
public:
	static unsigned int const max_value = 4;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	//unsigned int compare(const Attribute & rhs) const;
};

//the second way to measure class. literally the same as cClass except for the comparison function.
class cClass_M2 : public Attribute_Set_Mode < cClass_M2 > {
public:
	static const unsigned int max_value = 4;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	unsigned int compare(const Attribute & right_hand_side) const;
};


class cCoauthor : public Attribute_Set_Mode < cCoauthor >  {
	friend class cReconfigurator_Coauthor;
public:
	static unsigned int const max_value = 6;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
};

class cAsgNum;
class cAssignee_Data : public Attribute_Single_Mode < cAssignee_Data > {};

class cAssignee : public Attribute_Interactive_Mode <cAssignee, cAssignee_Data> {
public:
	static const unsigned int max_value = 6;
private:
	//static const map<string, std::pair<string, unsigned int>  > * assignee_tree_pointer; // this is a static membmer used in the comparison function.
	static map < const cAsgNum*, unsigned int > asgnum2count_tree;
	static bool is_ready;
public:
	cAssignee(const char * source = NULL ) {}
	unsigned int compare(const Attribute & rhs) const;
	//static void set_assignee_tree_pointer(const map<string, std::pair<string, unsigned int>  >& asgtree) {assignee_tree_pointer = & asgtree;}
	static void configure_assignee( const list <const cRecord *> & );
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			Attribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
};

class cAsgNum : public Attribute_Single_Mode<cAsgNum> {
public:
	cAsgNum(const char * source = NULL ){}
};

class cUnique_Record_ID : public Attribute_Single_Mode <cUnique_Record_ID> {
public:
	cUnique_Record_ID(const char * source = NULL ){}
};

class cApplyYear: public Attribute_Single_Mode<cApplyYear> {
public:
	cApplyYear(const char * source = NULL ){}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};

class cAppYear: public Attribute_Single_Mode<cAppYear> {
public:
	cAppYear(const char * source = NULL ){}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};

class cGYear: public Attribute_Single_Mode<cGYear> {
public:
	cGYear(const char * source = NULL ){}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};
class cCity: public Attribute_Single_Mode <cCity> {
public:
	cCity(const char * source = NULL ) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
	bool split_string(const char*);
	int exact_compare( const Attribute & rhs ) const { return this == & rhs; }
};

class cPatent: public Attribute_Single_Mode <cPatent> {
public:
	cPatent( const char * source = NULL){};
};



//=======

// template static variables.
// ALL THE FOLLOWING TEMPLATE MEMBER ARE DEFAULT VALUES.
// Declaration and default definition.
// Specialization should be implemented in the cpp file.
//template <typename Derived> unsigned int Attribute_Intermediary<Derived>::column_index_in_query;
//template <typename Derived> string Attribute_Intermediary<Derived>::column_name_in_query;
template <typename Derived> vector <unsigned int> Attribute_Basic<Derived>::interactive_column_indice_in_query;
template <typename Derived> bool Attribute_Basic<Derived>::bool_interactive_consistency_checked = false;
template <typename Derived> bool Attribute_Basic<Derived>::bool_is_enabled = false;
template <typename Derived> bool Attribute_Basic<Derived>::bool_comparator_activated = false;
template <typename Derived> set < string > Attribute_Intermediary<Derived>:: data_pool;
template <typename Derived> map < Derived, int > Attribute_Intermediary<Derived>:: attrib_pool;
template <typename Derived> pthread_rwlock_t Attribute_Intermediary<Derived>:: attrib_pool_structure_lock = PTHREAD_RWLOCK_INITIALIZER;
template <typename Derived> pthread_mutex_t Attribute_Intermediary<Derived>:: attrib_pool_count_lock = PTHREAD_MUTEX_INITIALIZER;

//declaration ( not definition ) of specialized template

template <> const string Attribute_Basic<cFirstname>::attrib_group;
template <> const string Attribute_Basic<cLastname>::attrib_group;
template <> const string Attribute_Basic<cMiddlename>::attrib_group;
template <> const string Attribute_Basic<cLatitude>::attrib_group;
template <> const string Attribute_Basic<cLongitude>::attrib_group;
template <> const string Attribute_Basic<cAssignee>::attrib_group;
template <> const string Attribute_Basic<cClass>::attrib_group;
template <> const string Attribute_Basic<cCoauthor>::attrib_group;
template <> const string Attribute_Basic<cClass_M2>::attrib_group;

template <typename Derived> const string Attribute_Basic<Derived>::attrib_group = INERT_ATTRIB_GROUP_IDENTIFIER;

//template <> const string Attribute_Intermediary<cLatitude>::interactive_column_names[];
template <> const string Attribute_Basic<cAssignee>::interactive_column_names[];
template <> const string Attribute_Basic<cLatitude >::interactive_column_names[];
template <> const string Attribute_Basic<cLongitude >::interactive_column_names[];
template <typename Derived> const string Attribute_Basic<Derived>::interactive_column_names[] = {};


//template <> const unsigned int Attribute_Intermediary<cLatitude>::num_of_interactive_columns;
template <> const unsigned int Attribute_Basic<cAssignee>::num_of_interactive_columns;
template <> const unsigned int Attribute_Basic<cLatitude >::num_of_interactive_columns;
template <> const unsigned int Attribute_Basic<cLongitude >::num_of_interactive_columns;
template <typename Derived> const unsigned int Attribute_Basic<Derived>::num_of_interactive_columns = 0;

//#endif /* DISAMBIGCUSTOMIZEDDEFS_H_ */

#endif /* DISAMBIGLIB_HPP_ */
