/*
 * DisambigCustomizedDefs.cpp
 *
 *  Created on: Apr 5, 2011
 *      Author: ysun
 */

#include "DisambigDefs.h"

template <> const string Attribute_Basic<cFirstname>::class_name = "Firstname";
template <> const string Attribute_Basic<cFirstname>::attrib_group = "Personal";


template <> const string Attribute_Basic<cLastname>::class_name = "Lastname";
template <> const string Attribute_Basic<cLastname>::attrib_group = "Personal";

template <> const string Attribute_Basic<cMiddlename>::class_name = "Middlename";
template <> const string Attribute_Basic<cMiddlename>::attrib_group = "Personal";

//template <> const string Attribute_Basic<cLatitude>::class_name = "Latitude";
//template <> const string Attribute_Basic<cLatitude>::interactive_column_names[] = {"Longitude", "Street", "Country"};
//template <> const unsigned int Attribute_Basic<cLatitude>::num_of_interactive_columns = 3;
//template <> const string Attribute_Basic<cLatitude>::attrib_group = "Patent";

template <> const string Attribute_Basic<cLatitude_Data >::class_name = "Latitude_Data";

template <> const string Attribute_Basic<cLatitude >::class_name = "Latitude";
template <> const string Attribute_Basic<cLatitude >::interactive_column_names[] = {"Longitude", "Street", "Country"};
template <> const unsigned int Attribute_Basic<cLatitude >::num_of_interactive_columns = 3;
template <> const string Attribute_Basic<cLatitude >::attrib_group = "Patent";

template <> const string Attribute_Basic<cLongitude_Data >::class_name = "Longitude_Data";
template <> const string Attribute_Basic<cLongitude>::class_name = "Longitude";
//template <> const string Attribute_Basic<cLongitude >::class_name = "Longitude";
template <> const string Attribute_Basic<cLongitude >::interactive_column_names[] = {"Latitude"};
template <> const unsigned int Attribute_Basic<cLongitude >::num_of_interactive_columns = 1;
template <> const string Attribute_Basic<cLongitude >::attrib_group = "Patent";


template <> const string Attribute_Basic<cStreet>::class_name = "Street";
// All the following are new
template <> const string Attribute_Basic<cState>::class_name = "State";
template <> const string Attribute_Basic<cZipcode>::class_name = "Zipcode";
template <> const string Attribute_Basic<cInvSeq>::class_name = "InvSeq";
//template <> const string Attribute_Basic<cAppDateStr>::class_name = "AppDateStr";
template <> const string Attribute_Basic<cAppDate>::class_name = "AppDate";
template <> const string Attribute_Basic<cinvnum_N>::class_name = "Invnum_N";
template <> const string Attribute_Basic<cinvnum>::class_name = "Invnum";


template <> const string Attribute_Basic<cCountry>::class_name = "Country";
//template <> const string Attribute_Basic<cCountry>::attrib_group = "Patent";

template <> const string Attribute_Basic<cClass>::class_name = "Class";
template <> const string Attribute_Basic<cClass>::attrib_group = "Patent";

template <> const string Attribute_Basic<cClass_M2>::class_name = "Class_Measure2";
template <> const string Attribute_Basic<cClass_M2>::attrib_group = "Patent";


template <> const string Attribute_Basic<cCoauthor>::class_name = "Coauthor";
template <> const string Attribute_Basic<cCoauthor>::attrib_group = "Patent";

template <> const string Attribute_Basic<cAssignee_Data >::class_name = "Assignee_Data";
template <> const string Attribute_Basic<cAssignee>::class_name = "Assignee";
template <> const string Attribute_Basic<cAssignee>::interactive_column_names[] = {"AsgNum"};
template <> const unsigned int Attribute_Basic<cAssignee>::num_of_interactive_columns = 1;
//const map<string, std::pair<string, unsigned int>  > * cAssignee::assignee_tree_pointer;
map < const cAsgNum*, unsigned int > cAssignee:: asgnum2count_tree;
bool cAssignee::is_ready = false;
template <> const string Attribute_Basic<cAssignee>::attrib_group = "Patent";

template <> const string Attribute_Basic<cAsgNum>::class_name = "AsgNum";

template <> const string Attribute_Basic<cUnique_Record_ID>::class_name = "Unique_Record_ID";

template <> const string Attribute_Basic<cApplyYear>::class_name = "ApplyYear";
template <> const string Attribute_Basic<cAppYear>::class_name = "AppYear";
template <> const string Attribute_Basic<cGYear>::class_name = "GYear";

template <> const string Attribute_Basic<cCity>::class_name = "City";

template <> const string Attribute_Basic<cPatent>::class_name = "Patent";


/*
 * Whenever overriding a comparison fucntion, it is extremely important to check if the comparator is activated. i.e.
 * Always keep the following statement in a comparison function:
 *
 * if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
 *
 *
 */


unsigned int cFirstname::previous_truncation = 0;
unsigned int cFirstname::current_truncation = 0;

/*
 * cFirstname::split_string does 3 things:
 * 1. Extract the first name from an input string, which is usually mixed with first name and middle name.
 * 	  i.e. Input string = "JOHN David WILLIAM", extracted string = "JOHN"
 * 2. Keep the original copy of the original input string.
 * 3. Save the first extracted string in data[0], and the second original copy in data[1].
 *
 */


bool cFirstname::split_string(const char *inputdata) {
	static const char delim = ' ';
	Attribute::split_string(inputdata);
	const string * psource = get_data().at(0);
	const size_t pos = psource->find(delim);
	vector < const string * > & data_alias = get_data_modifiable();
	if ( pos == string::npos )
		data_alias.push_back(psource);
	else {
		string to_push ( psource->begin(), psource->begin() + pos );
		data_alias.push_back(this->add_string( to_push) );
	}
	return true;
}


unsigned int cFirstname::compare(const Attribute & right_hand_side) const {
	// ALWAYS CHECK THE ACTIVITY OF COMPARISON FUNCTION !!
	if ( ! this->is_comparator_activated () )
		throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
	if ( this == & right_hand_side )
		return this->get_attrib_max_value();

	unsigned int res = 0;
	res = name_compare(* this->get_data().at(1), * right_hand_side.get_data().at(1), previous_truncation, current_truncation);
	if ( res > this->get_attrib_max_value() )
		res = this->get_attrib_max_value();
	return res;
}



/*
 * cMiddlename::split_string does 3 things:
 * 1. Extract the middle name from an input string, which is usually mixed with first name and middle name.
 * 	  i.e. Input string = "JOHN David WILLIAM", extracted string = "David WILLIAM"
 * 		   Input string = "JOHN", extracted string = ""
 *
 * 2. Extract the initials of middle names.
 * 	  i.e. Input string = "JOHN DAVID WILLIAM", middle initials = "DW"
 * 3. Save the first extracted string in data[0], and the second middle initial string in data[1].
 *
 *
 */



bool cMiddlename::split_string(const char *inputdata) {
	Attribute::split_string(inputdata);
	const string & source = * get_data().at(0);
	size_t pos = source.find(' ');
	string midpart;
	if ( pos == string::npos )
		midpart = "";
	else
		midpart = source.substr( pos + 1 );

	char initials[64];
	extract_initials(initials, midpart.c_str());
	const char * start = initials;
	vector < const string * > & data_alias = get_data_modifiable();
	string temp(start);
	data_alias.clear();
	data_alias.push_back(this->add_string( midpart) );
	data_alias.push_back(this->add_string( temp ) );
	return true;
}


/*
 * cMiddlename::compare:
 * Compare the extracted strings in data[0] to see whether they started with the same letter and whether one contains the other.
 * i.e.
 * "DAVID WILLIAM" vs "DAVID" = 3 ( max score)
 * "DAVID WILLIAM" vs "WILLIAM" = 0 (min score, not starting with the same letters)
 * "DAVID WILLIAM" vs "DAVE" = 0 ( either one does not container the other. )
 * "" vs "" = 2 ( both missing information )
 * "DAVID" vs "" = 1 ( one missing information )
 *
 */


unsigned int cMiddlename::compare(const Attribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	try {
		const cMiddlename & rhs = dynamic_cast< const cMiddlename & > (right_hand_side);
		unsigned int res = midnamecmp(* this->get_data().at(0), * rhs.get_data().at(0));
		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}



/*
 * cLatitude::compare:
 * Such comparison is complicated because cLatitude is interacted with cLongitude, cCountry , and possibly cStreet
 * One needs to know the structure of how the interactive data are stored, which is actually an assumed knowledge.
 *
 * If the distance calculated by latitude and longitude < 1 mile, score = 4 ( max score)
 * If the distance calculated by latitude and longitude < 10 mile, score = 3
 * If the distance calculated by latitude and longitude < 50 mile, score = 2
 * If the distance calculated by latitude and longitude > 50 mile, score = 1
 * If countries are different, score = 0;
 *
 */

unsigned int cLatitude::compare(const Attribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	check_if_reconfigured();
	try {
		unsigned int res = 0;
		const cLatitude & rhs = dynamic_cast< const cLatitude & > (right_hand_side);

		const Attribute* const & this_longitude = this->get_interactive_vector().at(0);
		const Attribute* const & rhs_longitude = rhs.get_interactive_vector().at(0);
		if ( this->get_data().size() != this_longitude->get_data().size() ) {
			std::cout << "Alignment error in latitude comparison: " << std::endl;
			this->print(std::cout);
			this_longitude->print(std::cout);
			throw cException_Interactive_Misalignment(this->get_class_name().c_str());
		}
		if ( rhs.get_data().size() != rhs_longitude->get_data().size() ) {
			std::cout << "Alignment error in latitude comparison: " << std::endl;
			rhs.print(std::cout);
			rhs_longitude->print(std::cout);
			throw cException_Interactive_Misalignment(this->get_class_name().c_str());
		}


		
		//latitude interacts with		{"Longitude", "Street", "Country"}; the sequence is important.


		unsigned int country_score = 0;
		if ( this == &rhs && this->is_informative() ) {
			res = max_value;
		}
		else {
			// Comparing country
			if ( this->get_interactive_vector().at(2) == rhs.get_interactive_vector().at(2) )
				country_score = 1;

			// Comparing street;
			//unsigned int street_score = 0;

			// Comparing Latitidue and longitude

			unsigned int latlon_score = 0;
			latlon_score = latloncmp ( * this->get_data().at(0), * this_longitude->get_data().at(0),
										* rhs.get_data().at(0), * rhs_longitude->get_data().at(0) );

			if ( country_score == 0 )
				res = 0;
			else
				res = latlon_score;
		}

		//correction for japanese
		if ( country_score == 1 && *this->get_interactive_vector().at(2)->get_data().at(0) == "JP" ) {
			const Attribute* const & this_street = this->get_interactive_vector().at(1);
			const Attribute* const & rhs_street = rhs.get_interactive_vector().at(1);
			if ( this_street == rhs_street && ( ! this_street->is_informative() ) )
				res -= 1;
		}

		if ( res > max_value )
			throw cException_Other("latitude error: score > max_value");

		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}


unsigned int cLongitude::compare(const Attribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	check_if_reconfigured();
	try {
		unsigned int res = 0;
		const bool exact_same = this->exact_compare(right_hand_side) == 1 ;
		if ( exact_same && this->is_informative())
			res = 1;

		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}


/*
 * cClass_M2::compare
 * A second way to score the "class" attribute.
 * Not in use now.
 */


unsigned int cClass_M2::compare(const Attribute & right_hand_side) const {
	const cClass_M2 & rhs = dynamic_cast< const cClass_M2 & > (right_hand_side);
	const unsigned int common = this->Attribute_Set_Mode <cClass_M2>::compare( rhs );
	const unsigned int this_size = this->attrib_set.size();
	const unsigned int rhs_size = rhs.attrib_set.size();

	const double factor = 1.0 * common * common / this_size / rhs_size;
	if ( factor > 0.3 )
		return 4;
	else if ( factor > 0.2 )
		return 3;
	else if ( factor > 0.1 )
		return 2;
	else if ( factor > 0.05 )
		return 1;
	else
		return 0;
}

/*
 * cCountry::compare
 * Not supposed to be used, because country attribute is mixed in the latitude comparison.
 *
 */

unsigned int cCountry::compare(const Attribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());

	if ( !this->is_informative() || ! right_hand_side.is_informative() )
		return 1;
	if ( this == & right_hand_side )
		return 2;
	else
		return 0;
}



/*
 * cAssignee::compare:
 * Comparison of assignee includes two steps:
 * 1. look up the assignee->asgnum(assignee number) tree, in order to check whether two assignees shared the same number.
 * 		If they do, they are believed to be the same, and the score of 5 is granted. If the assignee has fewer than 100 different patents, an additional 1 point is added.
 * 2. If their assignee numbers are different, they can still be the same assignee.
 *    In this case, a fairly crude jaro-winkler string comparison is used to score.
 *    Refer to the function jwcmp for more scoring information.
 *
 */

unsigned int cAssignee::compare(const Attribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	if ( ! cAssignee::is_ready )
		throw cException_Other("Trees for assignee comparison are not set up yet. Run cAssignee::configure_assignee first.");
	try {

		const cAssignee & rhs = dynamic_cast< const cAssignee & > (right_hand_side);
		//unsigned int res = asgcmp(this->get_data(), rhs.get_data(), assignee_tree_pointer);
		//unsigned int res = asgcmp ( * this->get_data().at(0), * rhs.get_data().at(0), assignee_tree_pointer);
		unsigned int res = 0;
		const cAsgNum * p = dynamic_cast < const cAsgNum *> (this->get_interactive_vector().at(0));
		if ( ! p )
			throw cException_Other("Cannot dynamic cast to cAsgNum *.");

		const cAsgNum * q = dynamic_cast < const cAsgNum *> (rhs.get_interactive_vector().at(0));
		if ( ! q )
			throw cException_Other("Cannot dynamic cast rhs to cAsgNum *.");

		if ( ! this->is_informative() || ! rhs.is_informative() ) {
			res = 1;
		}
		else if ( p != q ) {
			res = asgcmp(* this->get_data().at(0), * rhs.get_data().at(0));
		}
		else {
			res = 5;
			map < const cAsgNum *, unsigned int>::const_iterator t = cAssignee::asgnum2count_tree.find(p);
			if ( t == cAssignee::asgnum2count_tree.end() )
				throw cException_Other("AsgNum pointer is not in tree.");
			if ( t->second < 100 )
				++res;
		}

		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}

/*
 * The reason to override cCity::split_string from its base is because a city's name can include some delimiters in the default base implementation.
 *
 */

bool cCity::split_string(const char* source) {
	string temp (source);
	get_data_modifiable().push_back(this->add_string(temp));
	return true;
}
