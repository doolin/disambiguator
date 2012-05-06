/*
 * DisambigEngine.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
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
const string cBlocking_Operation::delim = "##";


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
		if ( std::find( inputvec.begin(), inputvec.end(), classlabel) == inputvec.end() ) {
			(*p)->deactivate_comparator();
		}
		else {
			(*p)->activate_comparator();
		}
	}
	cRecord::update_active_similarity_names();
}

/**
 * Aim: to truncate string as desired. See the explanation in the
 * header file for more details
 * 
 * Algorithm: simple string manipulation in C.
 */

string
cString_Truncate::manipulate( const string & inputstring ) const {

	if ( ! is_usable )
		throw cException_Blocking_Disabled("String Truncation not activated yet.");

	if ( 0 == nchar ) {
		if ( is_forward )
			return inputstring;
		else {
			return string("");
		}
	}

	if ( inputstring.size() == 0 )
		return inputstring;

	char * p = new char[ nchar + 1];
	const char * res = p;
	const char * source;
	if ( begin >= 0 && static_cast<unsigned int >(begin) < inputstring.size() )
		source = &inputstring.at(begin);
	else if ( begin < 0 && ( begin + inputstring.size() >= 0 ) )
		source = &inputstring.at( begin + inputstring.size() );
	else {
		delete [] p;
		throw cString_Truncate::cException_String_Truncation(inputstring.c_str());
	}

	if ( is_forward) {
		for ( unsigned int i = 0; i < nchar && *source !='\0'; ++i )
			*p++ = *source++;
		*p = '\0';
	}
	else {
		for ( unsigned int i = 0; i < nchar && source != inputstring.c_str() ; ++i )
			*p++ = *source--;
		*p = '\0';
	}
	string result (res);
	delete [] res;
	return result;
}

/*
 * Aim: to extract initials of each word in a string, maybe not starting from the first word.
 * See the explanation in the header file for more details
 * Algorithm: simple string manipulation in C.
 */

string cExtract_Initials::manipulate( const string & inputstring ) const {
	size_t pos, prev_pos;
	pos = prev_pos = 0;
	
	if ( inputstring.empty() )
		return string();
	list < char > tempres;
	do {		
		tempres.push_back(inputstring.at(prev_pos) );
		pos = inputstring.find(delimiter, prev_pos );
		prev_pos = pos + 1;
	} while ( pos != string::npos );
	
	
	const unsigned int word_count = tempres.size();
	if ( word_count >= starting_word ) {
		for ( unsigned int i = 0; i < starting_word; ++i )
			tempres.pop_front();
	}
	return string(tempres.begin(), tempres.end());
}


/*
 * Aim: to extract the first word of a string.
 * Algorithm: STL string operations.
 */
string 
cString_Extract_FirstWord::manipulate( const string & inputstring ) const {

	string res = inputstring.substr(0, inputstring.find(delimiter, 0));
	return res;
}


/*
 * Aim: constructors of cBlocking_Operation_Multiple_Column_Manipulate 
 * class. Look at the header file for more information.
 */
cBlocking_Operation_Multiple_Column_Manipulate::cBlocking_Operation_Multiple_Column_Manipulate (
    const vector < const cString_Manipulator * > & inputvsm, 
    const vector<string> & columnnames, 
    const vector < unsigned int > & di ) : vsm(inputvsm), attributes_names(columnnames) {

	if ( inputvsm.size() != columnnames.size() )
		throw cException_Other("Critical Error in cBlocking_Operation_Multiple_Column_Manipulate: size of string manipulaters is different from size of columns");
	for ( unsigned int i = 0; i < columnnames.size(); ++i ) {
		indice.push_back(cRecord::get_index_by_name( columnnames.at(i)));
		infoless += delim;
		pdata_indice.push_back( di.at(i));
	}
}

cBlocking_Operation_Multiple_Column_Manipulate::cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col ) {

	for ( unsigned int i = 0; i < num_col; ++i ) {
		vsm.push_back(*pinputvsm++);
		attributes_names.push_back(*pcolumnnames);
		indice.push_back(cRecord::get_index_by_name(*pcolumnnames++));
		infoless += delim;
		pdata_indice.push_back(*pdi ++);
	}
}

/**
 * Aim: to extract blocking information from a record pointer and 
 * returns its blocking id string.
 * 
 * Algorithm: call the polymorphic methods by cString_Manipulator 
 * pointers to create strings, and concatenate them.
 */
string 
cBlocking_Operation_Multiple_Column_Manipulate::extract_blocking_info(const cRecord * p) const {

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
string cBlocking_Operation_Multiple_Column_Manipulate::extract_column_info ( const cRecord * p, unsigned int flag ) const {
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
    const list < const cRecord * > & all_rec_pointers,
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



cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(const list < const cRecord * > & all_rec_pointers, const unsigned int coauthors)
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
 * and for any given const cRecord pointer p, look for the patent
 * attribute of p in * the map. If the patent attribute is not found,
 * insert p into the map as a key, and insert a list of const cRecord
 * pointers which includes p as * a value of the key; if the patent
 * attribute is found, find the corresponding value ( which is a list ),
 * and append p into the list.
 */
void 
cBlocking_Operation_By_Coauthors::build_patent_tree(const list < const cRecord * > & all_rec_pointers) {
	map < const cRecord *, cGroup_Value, cSort_by_attrib >::iterator ppatentmap;
	for ( list < const cRecord * >::const_iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p ) {
		ppatentmap = patent_tree.find(*p);
		if ( ppatentmap == patent_tree.end() ) {
			cGroup_Value temp ( 1, *p);
			patent_tree.insert( std::pair < const cRecord *, cGroup_Value > (*p, temp) );
		}
		else {
			ppatentmap->second.push_back(*p);
		}
	}
}

/*
 * Aim: to create binary tree of unique record id -> unique inventor id, also to allow fast search and insertion/deletion.
 * 		the unique inventor id is also a const cRecord pointer, meaning that different unique record ids may be associated with a same
 * 		const cRecord pointer that represents them.
 * Algorithm: clean the uinv2count and uid2uinv tree first.
 * 				For any cluster in the cCluser_Info object:
 * 					For any const cRecord pointer p in the cluster member list:
 * 						create a std::pair of ( p, d ), where d is the delegate ( or representative ) of the cluster
 * 						insert the pair into uid2uinv map.
 * 					End for
 * 				End for
 *			uinv2count is updated in the same way.
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
			const cRecord * value = q->get_cluster_head().m_delegate;
			map < const cRecord *, unsigned int >::iterator pcount = uinv2count_tree.find(value);
			if ( pcount == uinv2count_tree.end() )
				pcount = uinv2count_tree.insert(std::pair<const cRecord *, unsigned int>(value, 0)).first;

			for ( cGroup_Value::const_iterator r = q->get_fellows().begin(); r != q->get_fellows().end(); ++r ) {
				const cRecord * key = *r;
				uid2uinv_tree.insert(std::pair< const cRecord * , const cRecord *> (key, value ));
				++ ( pcount->second);
				++count;
			}
		}
	}
	std::cout << count << " nodes has been created inside the tree." << std::endl;
}

/*
 * Aim: to get a list of top N coauthors ( as represented by a const cRecord pointer ) of an inventor to whom prec ( a unique record id ) belongs.
 * Algorithm:
 * 		1. create a binary tree T( std::map ). Key = number of holding patents, value = const cRecord pointer to the unique inventor.
 * 		2. For any associated record r:
 * 				find r in the uinv2count tree.
 * 				if number of nodes in T < N, insert (count(r), r) into T;
 * 				else
 * 					if count(r) > front of T:
 * 						delete front of T from T
 * 						insert ( count(r), r );
 * 		3. return values in T.
 *
 */

cGroup_Value cBlocking_Operation_By_Coauthors::get_topN_coauthors( const cRecord * prec, const unsigned int topN ) const {
	const cGroup_Value & list_alias = patent_tree.find(prec)->second;
	map < unsigned int, cGroup_Value > occurrence_map;
	unsigned int cnt = 0;
	for ( cGroup_Value::const_iterator p = list_alias.begin(); p != list_alias.end(); ++p ) {
		if ( *p == prec )
			continue;

		map < const cRecord *, const cRecord * >::const_iterator puid2uiv = uid2uinv_tree.find(*p);
		if ( puid2uiv == uid2uinv_tree.end() )
			throw cException_Other("Critical Error: unique record id to unique inventer id tree is incomplete!!");
		const cRecord * coauthor_pointer = puid2uiv->second;

		map < const cRecord *, unsigned int >::const_iterator puinv2count = uinv2count_tree.find(coauthor_pointer);
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

string cBlocking_Operation_By_Coauthors::extract_blocking_info(const cRecord * prec) const {
	const cGroup_Value top_coauthor_list = get_topN_coauthors(prec, num_coauthors);
	// now make string
	const unsigned int firstnameindex = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int lastnameindex = cRecord::get_index_by_name(cLastname::static_get_class_name());

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

cReconfigurator_AsianNames::cReconfigurator_AsianNames(): country_index(cRecord::get_index_by_name(cCountry::static_get_class_name())),
								firstname_index(cRecord::get_index_by_name(cFirstname::static_get_class_name())),
								middlename_index(cRecord::get_index_by_name(cMiddlename::static_get_class_name())),
								lastname_index(cRecord::get_index_by_name(cLastname::static_get_class_name())){
	east_asian.push_back(string("KR"));
	east_asian.push_back(string("CN"));
	east_asian.push_back(string("TW"));
}

/*
 * Aim: reconfigure the asian name in accordance with the current scoring system.
 * Algorithm: check the country first. If the country is in [ "KR", "CN", "TW"], do this operation:
 *			1. create a vector of string, and save FULL first name into the vector. Then create a firstname attribute from the vector
 *			2. create a vector of string, and save "FULL firstname" + "FULL lastname" into the vector. Then create a middlename attribute from the vector.
 *			3. change the cRecord data such that the firstname pointer points to the newly created firstname object. So for the middle name.
 */

void cReconfigurator_AsianNames::reconfigure( const cRecord * p ) const {
	bool need_reconfigure = false;
	const string & country = *  p->get_attrib_pointer_by_index(country_index)->get_data().at(0) ;
	for ( register vector<string>::const_iterator ci = east_asian.begin(); ci != east_asian.end(); ++ci )
		if ( country == *ci ) {
			need_reconfigure = true;
			break;
		}

	if ( need_reconfigure == false )
		return;

	// do not change original attributes. add new ones.
	const cAttribute * cur_af = p->get_attrib_pointer_by_index(firstname_index);
	const string & fn_alias = * cur_af ->get_data().at(0);
	const vector <string> fn ( 2, fn_alias );
	const cAttribute * paf = cFirstname::static_clone_by_data(fn);

	const cAttribute * cur_am = p->get_attrib_pointer_by_index(middlename_index);
	const string & lnstr = * p->get_attrib_pointer_by_index(lastname_index)->get_data().at(0);
	const string mnstr ( fn_alias + "." + lnstr);
	const vector < string > mn(2, mnstr);
	const cAttribute * pam = cMiddlename ::static_clone_by_data(mn);

	cRecord * q = const_cast < cRecord * > (p);

	cur_af->reduce_attrib(1);
	cur_am->reduce_attrib(1);
	q->set_attrib_pointer_by_index(paf, firstname_index);
	q->set_attrib_pointer_by_index(pam, middlename_index);

}



cReconfigurator_Interactives::cReconfigurator_Interactives( const string & my_name,
		const vector < string > & relevant_attribs ) {
	my_index = cRecord::get_index_by_name(my_name);
	for ( vector<string>::const_iterator p = relevant_attribs.begin(); p != relevant_attribs.end(); ++p ) {
		unsigned int idx = cRecord::get_index_by_name(*p);
		relevant_indice.push_back(idx);
	}
}

void cReconfigurator_Interactives::reconfigure ( const cRecord * p ) const {

	vector < const cAttribute * > interact;
	for ( vector < unsigned int >::const_iterator i = relevant_indice.begin(); i != relevant_indice.end(); ++i ) {
		interact.push_back(p->get_attrib_pointer_by_index(*i));
	}
	const cAttribute * const & tp = p->get_attrib_pointer_by_index(my_index);
	const cAttribute * & cp = const_cast< const cAttribute * &> (tp);
	cp = tp->config_interactive(interact);
}


/*
 * Aim: constructor of cReconfigurator_Coauthor
 * Algorithm: The constructor calls cCoauthor::clear_data_pool() and cCoauthor::clear_attrib_pool(). So it is critical to remember that
 * construction of each cReconfigurator_Coauthor object will DESTROY all the information of any cCoauthor class and INVALIDATE any pointers
 * that point to a cCoauthor object. Therefore, creation of cReconfigurator_Coauthor objects shall NEVER happen during disambiguation.
 *
 */
cReconfigurator_Coauthor::cReconfigurator_Coauthor ( const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_authors) :
		reference_pointer ( & patent_authors), coauthor_index ( cRecord::get_index_by_name(cCoauthor::static_get_class_name())) {
	cCoauthor::clear_data_pool();
	cCoauthor::clear_attrib_pool();
}

/*
 * Aim: to recreate the whole coauthor database, and link them to appropriate pointers.
 * Algorithm: for each unique record id, find all other unique id records with which this one is associated. Then extract the FULL names
 * of those records, and save in cCoauthor data pool. After that, reset the pointer of this unique record to point to the newly built
 * attribute.
 */

void cReconfigurator_Coauthor :: reconfigure ( const cRecord * p ) const {
	static const string dot = ".";
	static const unsigned int firstnameindex = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	static const unsigned int lastnameindex = cRecord::get_index_by_name(cLastname::static_get_class_name());
	static const cString_Extract_FirstWord firstname_extracter;
	static const cString_Remove_Space lastname_extracter;

	map < const cRecord *, cGroup_Value, cSort_by_attrib >::const_iterator cpm;
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
	const cAttribute * np = cCoauthor::static_add_attrib(temp, 1);
	const cAttribute ** to_change = const_cast< const cAttribute ** > ( & p->get_attrib_pointer_by_index(coauthor_index));
	*to_change = np;

}

/*
 * Aim: to find a ratio that corresponds to a given similarity profile in a given similarity profile binary tree.
 * Algorithm: STL map find.
 */

double fetch_ratio(const vector < unsigned int > & ratio_to_lookup, const map < vector  < unsigned int>, double, cSimilarity_Compare > & ratiosmap ) {
	map < vector < unsigned int >, double, cSimilarity_Compare >::const_iterator p = ratiosmap.find( ratio_to_lookup);
	if ( p == ratiosmap.end())
		return 0;
	else
		return p->second;
}




std::pair<const cRecord *, double> disambiguate_by_set (
									const cRecord * key1, const cGroup_Value & match1, const double cohesion1,
									 const cRecord * key2, const cGroup_Value & match2, const double cohesion2,
									 const double prior,
									 const cRatios & ratio,  const double mutual_threshold ) {
	static const unsigned int firstname_index = cRecord::get_similarity_index_by_name(cFirstname::static_get_class_name());
	static const unsigned int midname_index = cRecord::get_similarity_index_by_name(cMiddlename::static_get_class_name());
	static const unsigned int lastname_index = cRecord::get_similarity_index_by_name(cLastname::static_get_class_name());
	static const unsigned int country_index = cRecord::get_index_by_name(cCountry::static_get_class_name());

	static const bool country_check = true;


	//prescreening.
	const bool prescreening = true;
	if ( prescreening ) {
		if ( country_check ) {
			const cAttribute * p1 = key1->get_attrib_pointer_by_index(country_index);
			const cAttribute * p2 = key2->get_attrib_pointer_by_index(country_index);
			if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
				return std::pair<const cRecord *, double> (NULL, 0);
		}


		vector < unsigned int > screen_sp = key1->record_compare(*key2);
		const double screen_r = fetch_ratio(screen_sp, ratio.get_ratios_map());
		const double screen_p = 1.0 / ( 1.0 + ( 1.0 - prior )/ prior / screen_r );
		if ( screen_p < 0.3 || screen_sp.at(firstname_index) == 0 || screen_sp.at(midname_index) == 0 || screen_sp.at(lastname_index) == 0 )
			return std::pair<const cRecord *, double> (NULL, 0);
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
				const cAttribute * p1 = (*p)->get_attrib_pointer_by_index(country_index);
				const cAttribute * p2 = (*q)->get_attrib_pointer_by_index(country_index);
				if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
					return std::pair<const cRecord *, double> (NULL, 0);
			}



			vector< unsigned int > tempsp = (*p)->record_compare(* *q);
			if ( tempsp.at(firstname_index) == 0 || tempsp.at(midname_index) == 0 || tempsp.at(lastname_index) == 0 )
				return std::pair<const cRecord *, double> (NULL, 0);


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
		return std::pair<const cRecord *, double> (NULL, probs_average);
	if ( ( ! partial_match_mode ) && interactive_average < threshold )
		return std::pair<const cRecord *, double> (NULL, interactive_average);


	double inter = 0;
	if ( partial_match_mode )
		inter = probs_average * match1_size * match2_size;
	else
		inter = interactive;

	const double probability = ( cohesion1 * match1_size * ( match1_size - 1 )
								+ cohesion2 * match2_size * ( match2_size - 1 )
								+ 2.0 * inter ) / ( match1_size + match2_size) / (match1_size + match2_size - 1 );
	//ATTENSION: RETURN A NON-NULL POINTER TO TELL IT IS A MERGE. NEED TO FIND A REPRESENTATIVE IN THE MERGE PART.
	return std::pair<const cRecord *, double>( key1, probability );

}



/*
 * Aim: Copy file.
 */

void copyfile(const char * target, const char * source) {
	std::cout << "Copying file " << source << " to " << target << std::endl;
	std::ifstream   input( source,std::ios::binary);
	std::ofstream   output( target,std::ios::binary);

	output   <<   input.rdbuf();

	std::cout << "File copy done." << std::endl;
}



//==========================================
//===========================================

/*
 * Aim: to fetch records from a txt format file into memory. This is a very important function.
 * Algorithm:
 * First of all, read the first line in the file. The first line should include all the information of each column. ie. They are usually the
 * column names. The format of the first line is "Column Name1,Column Name 2,Column Name3,...,Column Name Last". If the delimiter is not
 * comma, change the function variable "delim".
 * Second, check the argument "requested_columns" in all the columns, and record the indice of requested_columns
 * Third, starting from the second line to the end of the file, read relevant information with the help of delimiters and indice,
 * and save them in appropriate attributes.
 * Finally, do some concrete class related stuff, like setting static members and run reconfigurations.
 *
 */

bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns ){
	std::ifstream::sync_with_stdio(false);
	const char * delim = ",";	// this deliminator should never occur in the data.
	const unsigned int delim_size = strlen(delim);
	std::ifstream infile(txt_file);
	if ( ! infile.good()) {
		throw cException_File_Not_Found(txt_file);
	}
	string filedata;
	//getline(infile, filedata);
	//if ( filedata != raw_txt_authenticator )
	//	throw cException_File_Not_Found("Specified file is not a valid one.");
 
        //std::cout << "requested_columns.size: " << requested_columns.size << std::endl; 
        std::cout << "requested_columns[0]: " << requested_columns[0] << std::endl; 

	vector <string> total_col_names;
	getline(infile, filedata);
	register size_t pos, prev_pos;
	pos = prev_pos = 0;
	while (  pos != string::npos){
		pos = filedata.find(delim, prev_pos);
		string columnname;
		if ( pos != string::npos )
			columnname = filedata.substr( prev_pos, pos - prev_pos);
		else
			columnname = filedata.substr( prev_pos );
		total_col_names.push_back(columnname);
		prev_pos = pos + delim_size;
                std::cout << "columnname: " << columnname << std::endl;
	}
	cAttribute::register_class_names(requested_columns);
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


	cRecord::column_names = requested_columns;
	cAttribute ** pointer_array = new cAttribute *[num_cols];

	pos = prev_pos = 0;

	unsigned int position_in_ratios = 0;
        std::cout << "num_cols: " << num_cols << std::endl;
	for ( unsigned int i = 0; i < num_cols; ++i ) {

		const int pos_in_query = cAttribute::position_in_registry(cRecord::column_names[i]);
std::cout << "pos_in_query: " << pos_in_query << std::endl;
std::cout << "cRecord::column_names[i]: " << cRecord::column_names[i] << std::endl;
		if ( pos_in_query == -1 ) {
			for ( unsigned int j = 0; j < i; ++j )
				delete pointer_array[j];
			delete [] pointer_array;
			throw cException_ColumnName_Not_Found(cRecord::column_names[i].c_str());
		}
		else {
			pointer_array[i] = create_attribute_instance ( cRecord::column_names[i].c_str() );
                }
 
#if 0
		if ( cRecord::column_names[i] == cLongitude::class_name ) {
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cStreet::class_name ) {
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cCountry::class_name ) {
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cAsgNum::class_name ) {
			cAssignee::interactive_column_indice_in_query.push_back(i);
		}
#endif

std::cout << "i: " << i << std::endl;
// If this crashes, will need to add code in the function `create_attribute_instance`
		if ( pointer_array[i]->get_attrib_group() != string("None") ) {
std::cout << "pointer_array[i]->get_attrib_group(): " << pointer_array[i]->get_attrib_group() << std::endl;
			++position_in_ratios;
                }
	}

        std::cout << std::endl;

	// always do this for all the attribute classes
	for ( unsigned int i = 0; i < num_cols; ++i ) {
		pointer_array[i]->check_interactive_consistency(cRecord::column_names);
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
	const cAttribute * pAttrib;
	vector <const cAttribute *> temp_vec_attrib;
	vector <const cAttribute *> Latitude_interactive_attribute_pointers;
	while (getline(infile, filedata) ) {
		temp_vec_attrib.clear();


		for ( unsigned int i = 0; i < num_cols ; ++i ) {
			unsigned int column_location = 0;
			pos = prev_pos = 0;
			while ( column_location++ != requested_column_indice.at(i) ) {
				pos = filedata.find(delim, prev_pos);
				prev_pos = pos + delim_size;
			}
			pos = filedata.find(delim, prev_pos);
			if ( pos == string::npos ) {
				if ( prev_pos != filedata.size() )
					string_cache[i] = filedata.substr(prev_pos);
				else
					string_cache[i] = "";
			}
			else {
				string_cache[i] = filedata.substr(prev_pos, pos - prev_pos);
			}

			pointer_array[i]->reset_data(string_cache[i].c_str());
			pAttrib = pointer_array[i]->clone();	//HERE CREATED NEW CLASS INSTANCES.
			temp_vec_attrib.push_back(pAttrib);
		}

		cRecord temprec(temp_vec_attrib);
		source.push_back( temprec );

		++size;
		if ( size % base == 0 )
			std::cout << size << " records obtained." << std::endl;
	}
	std::cout << std::endl;
	std::cout << size << " records have been fetched from "<< txt_file << std::endl;
	cRecord::sample_record_pointer = & source.front();

	for ( unsigned int i = 0; i < num_cols; ++i )
		delete pointer_array[i];
	delete [] pointer_array;

	for ( list< cRecord>::iterator ci = source.begin(); ci != source.end(); ++ci )
		ci->reconfigure_record_for_interactives();

	std::cout << "Sample Record:---------" << std::endl;
	cRecord::sample_record_pointer->print();
	std::cout << "-----------------" << std::endl;

	return true;
}

cAttribute * create_attribute_instance ( const string & id ) {
	cAttribute *p = NULL;
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
//		p = new cAppDateStr;
//	}
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


const cRecord_Reconfigurator * generate_interactive_reconfigurator( const cAttribute * pAttrib) {
	vector <string > linked_attribs (pAttrib->get_interactive_class_names());
	string my_name = pAttrib->get_class_name();
	//ATTENTION: OBJECT IS ON HEAP.
	const cRecord_Reconfigurator * preconfig = new cReconfigurator_Interactives( my_name, linked_attribs );
	return preconfig;
}

void cRecord::reconfigure_record_for_interactives() const {
	for ( vector <const cAttribute *>::const_iterator cipa = vector_pdata.begin(); cipa != vector_pdata.end(); ++cipa ) {
		(*cipa)->reconfigure_for_interactives( this);
	}
}

void reconfigure_interactives ( const cRecord_Reconfigurator * pc, const cRecord * pRec) {
	pc->reconfigure(pRec);
}



void cAssignee::configure_assignee( const list < const cRecord *> & recs) {
	static const unsigned int asgnumidx = cRecord::get_index_by_name(cAsgNum::static_get_class_name());

	for ( list< const cRecord *>::const_iterator p = recs.begin(); p != recs.end(); ++p ) {
		const cAsgNum * pasgnum = dynamic_cast < const cAsgNum *> ( (*p)->get_attrib_pointer_by_index(asgnumidx) );
		if ( ! pasgnum ) {
			throw cException_Other("Cannot perform dynamic cast to cAsgNum.");
		}
		++cAssignee::asgnum2count_tree[pasgnum];
	}

	cAssignee::is_ready = true;
}


void build_patent_tree( map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree , const list < const cRecord * > & all_rec_pointers ) {
	map < const cRecord *, cGroup_Value, cSort_by_attrib >::iterator ppatentmap;
	for ( list < const cRecord * >::const_iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p ) {
		ppatentmap = patent_tree.find(*p);
		if ( ppatentmap == patent_tree.end() ) {
			cGroup_Value temp ( 1, *p);
			patent_tree.insert( std::pair < const cRecord *, cGroup_Value > (*p, temp) );
		}
		else {
			ppatentmap->second.push_back(*p);
		}
	}
}

void build_patent_tree( map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree , const list < cRecord > & all_records ) {
	list < const cRecord *> all_pointers;
	for ( list < cRecord >::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_pointers.push_back(&(*p));
	build_patent_tree(patent_tree, all_pointers);
}


string check_file_existence(const string & description) {
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
