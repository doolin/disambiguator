/*
 * DisambigEngine.h
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
 */

#ifndef PATENT_ENGINE_H
#define PATENT_ENGINE_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "record.h"
#include "Threading.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

/*
 * cSort_by_attrib:
 * A functor for comparison of attributes in associated containers such as map or set.
 * bool operator () (const Record * prec1, const Record *prec2 ) const: comparison between two records pointers based on their content.
 * 			The conparison content depends on the internal information of the cSort_by_attrib object. i.e. attrib_index;
 *
 * Example:
 * 1. cSort_by_attrib obj1( "Patent" ); this will create an instance obj1 which compares two Record pointers based on the "Patent" information column.
 *    map < const Record *, VALUE_TYPE, cSort_by_attrib > m1(obj1); will create a map (binary tree) whose key is const Record pointer, and
 *    		the way of sorting is through comparison of "Patent" attribute. Binary tree is ideal for fast search, insertion and deletion.
 *
 */

class cSort_by_attrib {
private:
	const unsigned int attrib_index;	//attrib_index is the column index on which the Record's comparison depends.
public:
	bool operator () (const Record * prec1, const Record *prec2 ) const {
		const Attribute * attr1 = prec1->vector_pdata.at(attrib_index);
		const Attribute * attr2 = prec2->vector_pdata.at(attrib_index);

		return attr1->get_data().at(0) < attr2->get_data().at(0);
	};
	cSort_by_attrib(const unsigned int i): attrib_index(i) {};
	cSort_by_attrib(const string & attrib_name): attrib_index(Record::get_index_by_name(attrib_name) ) {};
};



//=====================================================

/*
 * cString_Manipulator:
 * 	- cString_Remain_Same
 * 	- cString_Remove_Space
 * 	- cString_Truncate
 * 		-- cString_NoSpace_Truncate
 * 	- cExtract_Initials
 * 	- cString_Extract_FirstWord
 *
 * cString_Manipulator is a hierarchy of of string operation functors. The reason to create such hierarchy is to allow polymorphism.
 * Subclasses override the virtual function "string manipulate ( const string & input ) const " with their own implementations.
 *
 * To use, one should create an object of a subclass, and call the "manipulate" function.
 * To add more user defined string manipulators, one should simply create a class that inherits the cString_Manipulator class, and override the manipulate function.
 *
 */

class cString_Manipulator{
private:

public:
	virtual string manipulate(const string & inputstring) const = 0;
	virtual ~cString_Manipulator() {};
	virtual cString_Manipulator * clone () const = 0;
};

/*
 * cString_Remain_Same:
 * As is indicated by the name, this class returns the raw string it obtains.
 *
 */

class cString_Remain_Same : public cString_Manipulator {
private:
	cString_Manipulator * clone () const { return new cString_Remain_Same(*this);}
public:
	string manipulate(const string & inputstring ) const { return inputstring;}

};


/*
 * cString_Remove_Space:
 * This class remove all the white spaces of the input string and returns a cleaned one.
 * i.e. Input = "THIS IS AN   EXAMPLE  ". Return value = "THISISANEXAMPLE".
 *
 */

class cString_Remove_Space : public cString_Manipulator {
private:
	static const char delimiter = ' ';
	cString_Manipulator * clone () const { return new cString_Remove_Space(*this);}
public:
	string manipulate( const string & inputstring ) const {
		string result = inputstring;
		result.erase(std::remove_if (result.begin(), result.end(), ::isspace), result.end() );
		return result;
	};
	explicit cString_Remove_Space(){};
};

/*
 * cString_Truncate:
 * This class is more often used for string operation. Understanding of the member is desirable.
 *
 * Private:
 * int begin: location of the starting point for truncation. Non-negative value means the position of starting character is from the beginning,
 * 			  and negative value means the position of starting character is from the end of the string.
 * unsigned int nchar: number of characters to extract. if nchar == 0, all the string will be extracted. If nchar > length of string, only the string part will be kept.
 * bool is_forward: indicating the direction to truncation. true = forward ( left to right ), false = backward ( right to left )
 * bool is_usable: internal data of the class. false = the object is not prepared yet. true = ready to use.
 *
 *
 * WARNING: A COMBINATION OF NCHAR = 0 AND IS_FORWARD = FALSE WILL RETURN AN EMPTY STRING.
 *
 *
 * Public:
 * 	explicit cString_Truncate(): default constructor.
 *  void set_truncater( const int inputbegin, const unsigned int inputnchar, const bool inputforward): configure the object.
 *  string manipulate( const string & inputstring ) const: implementation of the extraction operation. Defined in cpp file.
 *
 * Example:
 *  cString_Truncate stobj; //created an instance
 *  stobj.set_truncater(0, 5, true) : starting position = 0 (head of the string), extraction length = 5, direction = forward.
 *  stobj.manipulate ("ERIC") returns "ERIC".
 *  stobj.manipulate ("JOHNSON") returns "JOHNS";
 *  stobj.set_truncater(0, 0, true) : starting position = 0 (head of the string), extraction length = full length, direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "JOHNSON";
 *  stobj.set_truncater(-6, 2, true) : starting position = -6, extraction length = 2, direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "OH";
 *  stobj.set_truncater(-6, 2, false) : starting position = -6, extraction length = 2, direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "HO";
 *  stobj.set_truncater(4, 3, false) : starting position = 4, extraction length = 4, direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "SNH";
 *  stobj.set_truncater(4, 0, false) : starting position = 4, extraction length = 0, direction = backward.  --- read the Warning part.
 *  stobj.manipulate ("JOHNSON") returns "";
 *
 */


class cString_Truncate: public cString_Manipulator {
private:
	int begin;
	unsigned int nchar;
	bool is_forward;
	bool is_usable;

	class cException_String_Truncation: public cAbstract_Exception {
	public:
		cException_String_Truncation (const char * errmsg) : cAbstract_Exception(errmsg) {};
	};
	cString_Manipulator * clone () const { return new cString_Truncate(*this);}
public:
	explicit cString_Truncate(): is_usable (false) {};
	void set_truncater( const int inputbegin, const unsigned int inputnchar, const bool inputforward) {
		begin = inputbegin;
		nchar = inputnchar;
		is_forward = inputforward;
		is_usable = true;
	}
	string manipulate( const string & inputstring ) const;
};

/*
 * cString_NoSpace_Truncate:
 * The functionality of the class is similar to the above cString_Truncate.
 * The only difference is that it removes space first and then truncates as directed.
 *
 */

class cString_NoSpace_Truncate: public cString_Truncate {
private:
	const cString_Remove_Space ns;
	cString_Manipulator * clone () const { return new cString_NoSpace_Truncate(*this);}
public:
	string manipulate ( const string & inputstring ) const {
		string temp = ns.manipulate(inputstring);
		return cString_Truncate::manipulate(temp);
	}
};


/*
 * cExtract_Initials:
 * Extract the initials from the string, starting from the (starting_word)th word.
 *
 * Example:
 * cExtract_Initials eiobj(3);	//create an instance eiobj, setting the starting_word = 3;
 * eiobj.manipulate("THIS IS AN EXAMPLE, YOU KNOW.") returns "EYK".
 *
 */

class cExtract_Initials : public cString_Manipulator {
private:
	const unsigned int starting_word;
	static const char delimiter = ' ';
	cString_Manipulator * clone () const { return new cExtract_Initials(*this);}
public:
	string manipulate (const string & inputstring ) const;
	explicit cExtract_Initials (const unsigned int start ): starting_word(start) {};
};


/*
 * cString_Extract_FirstWord:
 * Extract the first word of the input string.
 *
 * cString_Extract_FirstWord sefobj;
 * sefobj.manipulate("THOMAS DAVID ANDERSON") returns "THOMAS"
 */

class cString_Extract_FirstWord : public cString_Manipulator {
private:
	static const char delimiter = ' ';
	cString_Manipulator * clone () const { return new cString_Extract_FirstWord(*this);}
public:
	string manipulate (const string & inputstring ) const;
};

//==========================================================

/*
 * cBlocking_Operation
 * 	- cBlocking_Operation_Column_Manipulate
 * 	- cBlocking_Operation_Multiple_Column_Manipulate
 *
 * cBlocking_Operation reads data from a const Record pointer, and extract information to create a string which represents the blocking id to which the record belongs.
 *
 * Protected:
 * 		string infoless: this string represents the string that has delimiters ONLY. i.e. no useful information at all.
 * 						Usually, the variable is changed by concrete subclasses.
 *
 * Public:
 * 		const string & get_useless_string () const: returns the infoless string.
 * 		static const string delim: the separator between columns. Defined in cpp file. Will be used by other modules.
 * 		virtual string extract_blocking_info(const Record *) const = 0: pure virtual function to extract information from a Record pointer.
 * 		virtual string extract_column_info ( const Record *, unsigned int flag ) const:
 * 				virtual function that extract information from the (flag)th column.
 *  	virtual unsigned int num_involved_columns() const: return number of columns that the extraction operates on.
 *
 * Example:
 * 		Record recobj: firstname = "JACK", lastname = "SMITH", patent = "12345A"
 * 		if cBlocking_Operation::delim == "###", then
 * 			extract_blocking_info ( &recobj ) probably returns "J###SMI###" if the blocking operation
 * 											just wants the first char of firstname and 3 chars from lastname.
 * 			extract_column_info ( & recobj, 1 ) probably returns "SMI" if the information of the second column (which is lastname ) is desired.
 * 			num_involved_column() probably returns 2. ( firstname + lastname).
 * 			get_useless_string() returns "######".
 *
 */

class cBlocking_Operation {
protected:
	string infoless;
public:
	const string & get_useless_string () const { return infoless;}
	static const string delim;
	virtual string extract_blocking_info(const Record *) const = 0;
	virtual string extract_column_info ( const Record *, unsigned int flag ) const { throw cException_Other ("Function not defined yet.");}
	virtual ~cBlocking_Operation() {};
	virtual unsigned int num_involved_columns() const { return 1;}
};

/*
 * cBlocking_Operation_Column_Manipulate:
 * This is a subclass of cBlocking_Operation. Usage of the class is described in its base class.
 *
 * Example:
 * const cString_Truncate stobj (0, 3, true);
 * cBlocking_Operation_Column_Manipulate bocmobj ( stobj, cLastname::get_class_name() );
 * 		// set up a bocmobj instance of cBlocking_Operation_Column_Manipulate, which takes a cString_Truncate object as the string manipulater,
 * 		// and applies the string manipulation on the "Last name" column.
 * Assuming a record recobj: Firstname = "JOHN", lastname = "FLEMING", patent = "1234Q", Country = "CA", ... ...
 * bocmobj.extract_blocking_info( & recobj ) returns "FLE";
 *
 *
 * However, this concrete class is not used often, because the next class (cBlocking_Operation_Multiple_Column_Manipulate ) is more widely used.
 *
 */
class cBlocking_Operation_Column_Manipulate: public cBlocking_Operation {
private:
	const cString_Manipulator & sm;
	const unsigned int column_index;
public:
	explicit cBlocking_Operation_Column_Manipulate(const cString_Manipulator & inputsm, const string & colname)
			: sm(inputsm), column_index(Record::get_index_by_name(colname)) { infoless = delim;}
	string extract_blocking_info(const Record * p) const {return sm.manipulate( * p->get_data_by_index(column_index).at(0));}
	string extract_column_info ( const Record * p, unsigned int flag ) const { return extract_blocking_info(p);}
};



/*
 * cBlocking_Operation_Multiple_Column_Manipulate:
 * This is a subclass of cBlocking_Operation, which extracts information from several columns and returns a block specifier string.
 *
 * Private:
 * 		vector < const cString_Manipulator * > vsm: the vector of string manipulator pointers.
 * 		vector < unsigned int > indice: the vector of column indice from which the manipulators extract information, respectively.
 * 		vector < const unsigned int * > pdata_indice: the vector of the const interger pointers indicating the positions of data in the data vector in Attribute on which the extractions take place.
 *
 * Public:
 * 	cBlocking_Operation_Multiple_Column_Manipulate (const vector < const cString_Manipulator * > & inputvsm, const vector<string> & columnnames, const vector < unsigned int > & di )
 * 		: This is a constructor of the class. inputvsm = the vector of string manipulator pointers
 * 			columnnames = the vector of strings which reprensents the name of columns that the extractions will be applied on, respectively.
 * 			di = the vector of indice in the Attribute::data, which will serve as the source string pointers.
 * 			NOTE: di SHOULD NOT BE DESTRUCTED BEFORE THE cBlocking_Operation_Multiple_Column_Manipulate object is discarded.
 * cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col ):
 * 			This is another constructor of the class object. pinputvsm = pointer of the string manipulator pointers array
 * 			pcolumnnames = pointer of the strings array. pdi = pointer of the Attribute::data indice array. num_col = number of involed columns.
 * 			NOTE: pdi SHOULD NOT BE DESTRUCTED BEFORE THE cBlocking_Operation_Multiple_Column_Manipulate object is discarded.
 *	string extract_blocking_info(const Record * p) const: extracts information and returns a string.
 *	string extract_column_info ( const Record * p, unsigned int flag ) const: extracts information from specified column and returns a string
 *	unsigned int num_involved_columns() const : return number of involved columns.
 *
 *
 * Example:
 *
 * cString_Remove_Space rsobj; //create a space removal string manipulator
 * cString_NoSpace_Truncate nstobj; // create a space removal and string truncation string manipulator
 * nstobj.set_truncater(2, 4, true); // set the nstobj to truncate strings in the way such that the truncation starts from the 3rd char
 * 									// and grabs 4 chars (if exists) from the forward direction. ie. nstobj.manipulate("EXAMPLES") returns "AMPL"
 * vector < const cString_Manipulator * > vec_strman;	//initialize a string manipulator pointer vector
 * vec_strman.push_back( &rsobj );
 * vec_strman.push_back( &rsobj );
 * vec_strman.push_back( & nstobj ); // the vector now has 3 elements.
 *
 * vector < string > vec_label; //initialize a vector container to store column names on which the string operations take place.
 * vec_label.push_back(cFirstname::static_get_class_name() );
 * vec_label.push_back(cAssignee::static_get_class_name() );
 * vec_label.push_back(cLastname::static_get_class_name() );	// the vector also has 3 elements. Pay attention to the sequence!
 *
 * unsigned int data_position[] = { 1, 0, 2 };	// initialize a data position array.
 * vector < const unsigned int * > vec_di ; // initialize a vector to used the data position array
 * vec_di.push_back( data_position );
 * vec_di.push_back(data_position + 1);
 * vec_di.push_back(data_position + 2);	// another vector that has 3 elements. Also notice the sequence.
 *
 * cBlocking_Operation_Multiple_Column_Manipulate mcmobj ( vec_strman, vec_label, vec_di );	//create a multiple column blocking info extractor.
 *
 * Assuming there is a Record object, recobj. As is described above, recobj contains a vector of const Attribute pointers.
 * Say within the many Attribute pointers that recobj has, 3 of them are:
 * 1. const Attribute * pfirstname: pointing to an attribute whose data vector contains 3 string pointers:
 * 		1.1 pfirstname->data[0] points to "ALEXANDER"
 * 		1.2 pfirstname->data[1] points to "NICOLAS"
 * 		1.3 pfirstname->data[2] points to "ALEXNICOLE".
 * Then the first string manipulator, rsobj, will look for the string of data[1] and returns "NICOLAS".
 *
 * 2. const Attribute * passignee: pointing to an attribute whose data vector contains 4 string pointers:
 * 		2.1 passignee->data[0] points to "MICROSOFT CORP, LTD"
 * 		2.2 passignee->data[1] points to "MICROSOFT CORPORATION, LTD"
 * 		2.3 passignee->data[2] points to "MICROSOFT CHINA"
 * 		2.4 passignee->data[3] points to "MICROSOFT SHANGHAI, CHINA"
 * Then the second string manipulator, also rsobj, will look for the string of data[0] and returns "MICROSOFTCORP,LTD"
 *
 * 3. const Attribute * plastname: pointing to an attribute whose data vector contains 4 string pointers:
 * 		3.1 plastname->data[0] points to "JOHNSON"
 * 		3.2 plastname->data[1] points to "JOHNSON JR"
 * 		3.3 plastname->data[2] points to "JOHN STONE"
 * 		3.3 plastname->data[3] points to "II JOHNSTON"
 * Then the 3rd string manipulator, nstobj, will look for the string of data[2] and, by removing space and truncating, returns "HNST";
 *
 * Assuming that the cBlocking_Operation::delim is "@", then
 * mcmobj.extract_blocking_info( & recobj ) returns "NICOLAS@MICROSOFTCORP,LTD@HNST@"
 * and
 * mcmobj.extract_column_info (& recobj, 2) returns "HNST".
 *
 *
 */

class cBlocking_Operation_Multiple_Column_Manipulate : public cBlocking_Operation {
private:
	vector < const cString_Manipulator * > vsm;
	vector < unsigned int > indice;
	vector < string > attributes_names;
	vector < unsigned int > pdata_indice;

public:
	cBlocking_Operation_Multiple_Column_Manipulate (const vector < const cString_Manipulator * > & inputvsm, const vector<string> & columnnames, const vector < unsigned int > & di );
	cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col );
	string extract_blocking_info(const Record * p) const;
	string extract_column_info ( const Record * p, unsigned int flag ) const;
	unsigned int num_involved_columns() const { return vsm.size();}
	void reset_data_indice( const vector < unsigned int > & indice );
	const vector < string > & get_blocking_attribute_names() const { return attributes_names;}
	const vector < const cString_Manipulator * > & get_blocking_string_manipulators() const { return vsm;}
};

class cCluster_Info;	//forward declaration.

/*
 * cBlocking_Operation_By_Coauthor:
 * This is a subclass of cBlocking_Operation. It was used to find the top N unique inventors' names of a given record.
 * Although it is not used any more for blocking, it does, however, provide other functionalities such as :
 * 1. sort records by patent through a binary tree. Ideal for fast search
 * 2. map unique record id to unique inventor id. This can be regarded as an expression of result of disambiguation.
 * 3. map unique inventor id to number of records (patents) that the inventor holds.
 * Therefore, this class is actually working beyond its original design purpose. And one should be familiar with the class members.
 *
 * Private:
 * 		map < const Record *, cGroup_Value, cSort_by_attrib > patent_tree: a binary tree to sort records by patent information.
 * 				key = const Record pointer, value = list < const Record * >, comparison function = an object of cSort_by_attrib, which includes the column information.
 *		map < const Record *, const Record * > uid2uinv_tree: a binary tree that maps a unique record pointer to its unique inventor record pointer.
 *		map < const Record *, unsigned int > uinv2count_tree: a binary tree that maps the unique inventor record pointer to the number of its associated patents.
 *		const unsigned int num_coauthors: number of coauthors to extract for blocking. Usually it should not be too large ( like 10 ).
 *
 *		void build_patent_tree(const list < const Record * > & allrec): reads a list of const Record pointers and build patent tree.
 *				NOTE THAT the comparison function for the tree is determined in the constructor.
 *		cGroup_Value get_topN_coauthors( const Record * pRec, const unsigned int topN) const:
 *				find the top N most prolific coauthors of the input pRec record pointer, and return a list of const Record pointers.
 *
 * Public:
 *		cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec, const cCluster_Info& cluster, const unsigned int coauthors):
 *				initialize a class object. allrec = a complete list of const Record pointers.
 *				cluster = a cCluster_Info object, which contains all the information of clustering from the previous disambiguation.
 *				coauthors = number of coauthors for blocking information extraction.
 *		cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec, const unsigned int coauthors): the second constructor.
 *		string extract_blocking_info(const Record * prec) const; extract blocking string for the Record to which prec points, and returns a string.
 *		void build_uid2uinv_tree( const cCluster_Info & source): build an internal unique record id to unique inventor id map from a cCluster_Info object.
 *		const map < const Record *, cGroup_Value, cSort_by_attrib > & get_patent_tree() const : get the reference of const patent tree
 *		map < const Record *, const Record * > & get_uid2uinv_tree(): get the reference of the unique record id to unique inventor id tree.
 *																		NOTE: the referenced tree is not const so it is modifiable.
 *
 *
 * Example:
 *
 * list < const Record * > complete_list;	//initialize a list
 * Do something here to add elements into the list.
 * cBlocking_Operation_By_Coauthors bobcobj ( complete_list, 2 ); //create an instance of cBlocking_Operation_By_Coauthors
 * 													// and set the maximum most prolific coauthors to 2, for blocking information extraction.
 * bobcobj.build_uid2uinv_tree ( Good_Cluster ); // assuming Good_Cluster is a cCluster_Info object. This builds an internal uid2uinv tree in bobcobj.
 * Now bobcobj's patent_tree and uid2uinv tree are ready to use.
 * bobcobj.extract_blocking_info(prec) returns the string of the full name concatenation of prec's two most prolific coauthors.
 * for example: "KIA.SILVERBROOK##GEORGE.SPECTOR##", assuming the delimiter is "##";
 *
 */
class cBlocking_Operation_By_Coauthors : public cBlocking_Operation {
private:
	map < const Record *, cGroup_Value, cSort_by_attrib > patent_tree;
	map < const Record *, const Record * > uid2uinv_tree;
	map < const Record *, unsigned int > uinv2count_tree;

	const unsigned int num_coauthors;

	void build_patent_tree(const list < const Record * > & allrec);
	cGroup_Value get_topN_coauthors( const Record *, const unsigned int topN) const;

public:
	cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec, const cCluster_Info& cluster, const unsigned int coauthors);
	cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec, const unsigned int num_coauthors);
	string extract_blocking_info(const Record *) const;
	void build_uid2uinv_tree( const cCluster_Info &);
	const map < const Record *, cGroup_Value, cSort_by_attrib > & get_patent_tree() const {return patent_tree;}
	map < const Record *, const Record * > & get_uid2uinv_tree() { return uid2uinv_tree;}

};


/*
 * Record_Reconfigurator
 * 	- cReconfigurator_AsianNames
 * 	- cReconfigurator_Latitude_Interactives
 * 	- cReconfigurator_Coauthor
 *
 * The record reconfigurator hierarchy is used to modify the data after initial loading from text files.
 * For some reasons, initial data are not ideal for disambiguation, and modification of such data is necessary.
 * It is also critical to reconfigure Record objects when interactions between attributes exist.
 * To add more concrete classes, simply inherit the Record_Reconfigurator class and implement the "void reconfigure ( const Record * ) const" virtual function.
 * To use a concrete Record_Reconfigurator class, create an object of the class and treat the object as a function, as shown below.
 *
 */


class Record_Reconfigurator {
public:
	virtual void reconfigure ( const Record * ) const = 0;
	void operator () ( Record & r ) const  {this->reconfigure(&r);}
	void operator () ( const Record * p) const { this->reconfigure(p); }
	virtual ~Record_Reconfigurator(){}
};

/*
 * cReconfigurator_AsianNames:
 * This class is specifically designed for east asian names. Generally, east asian names do not have middle names. For example,
 * "MING LIANG.ZHANG" of China has the first name as "MING LIANG" instead of "MING", whereas the originally regarded "LIANG" as middle name actually
 * is not middle name. Moreover, their first names and last names are usually extremely similar in terms of spelling. \
 * Therefore, to accommodate reasonable comparisons, their first names are set to be the original strings from the raw text data,
 * and their middle names are set to be the combination of first names and last names.
 *
 * Private:
 * 		const unsigned int country_index: index of country column in the record vector
 * 		const unsigned int firstname_index: index of first name column in the record vector
 * 		const unsigned int middlename_index: index of middle name column in the record vector
 * 		const unsigned int lastname_index: index of last name column in the record vector
 * 		vector < string > east_asian: string of east asian country ids.
 * 		const cString_Remove_Space rmvsp: a sring manipulator. Perhaps not used here.
 *
 * Public:
 * 		cReconfigurator_AsianNames(): constructor
 * 		void reconfigure( const Record * ) const: virtual function.
 *
 *
 * For example: assuming there is a record object, recobj, which has the cFirstname attribute, cMiddlename and cLastname attribute shown as below.
 * Before reconfiguration:
 *  1. const Attribute * pfirstname: pointing to an attribute whose data vector contains 2 string pointers:
 * 		1.1 pfirstname->data[0] points to "MING LIANG"
 * 		1.2 pfirstname->data[1] points to "MING"
 *
 *  2. const Attribute * pmiddlename: pointing to an attribute whose data vector contains 2 string pointers:
 * 		2.1 pfirstname->data[0] points to "MING LIANG"
 * 		2.2 pfirstname->data[1] points to "LIANG"
 *
 *  3. const Attribute * plasname: pointing to an attribute whose data vector contains 2 string pointers:
 * 		3.1 pfirstname->data[0] points to "ZHANG"
 * 		3.2 pfirstname->data[1] points to "ZHANG"
 *
 *  By applying the reconfiguration through:
 *  cReconfigurator_AsianNames rcfgobj; //create a reconfigurator object.
 *  rcfgobj ( & recobj ); // rcfgobj is a cReconfigurator_AsianNames object
 *  We now have:
 *
 * 	After reconfiguration:
 *  1. const Attribute * pfirstname: pointing to an attribute whose data vector contains 2 string pointers:
 * 		1.1 pfirstname->data[0] points to "MING LIANG"
 * 		1.2 pfirstname->data[1] points to "MING LIANG"
 *
 *  2. const Attribute * pmiddlename: pointing to an attribute whose data vector contains 2 string pointers:
 * 		2.1 pfirstname->data[0] points to "MING LIANG.ZHANG"
 * 		2.2 pfirstname->data[1] points to "MING LIANG.ZHANG"
 *
 *  3. const Attribute * plasname: pointing to an attribute whose data vector contains 2 string pointers:
 * 		3.1 pfirstname->data[0] points to "ZHANG"
 * 		3.2 pfirstname->data[1] points to "ZHANG"
 *
 */


class cReconfigurator_AsianNames : public Record_Reconfigurator {
private:
	const unsigned int country_index;
	const unsigned int firstname_index;
	const unsigned int middlename_index;
	const unsigned int lastname_index;
	vector < string > east_asian;
	const cString_Remove_Space rmvsp;
public:
	cReconfigurator_AsianNames();
	void reconfigure( const Record * ) const;
};



/*
 * cReconfigurator_Interactives:
 * This class is used to modify the objects to connect with its associated other Attribute objects.
 *
 * Example:
 * vector < string > links;
 * links.push_back("Longitude");
 * links.push_back("Street");
 * links.push_back("Country");
 * cReconfigurator_Interactives obj ("Latitude", links);	// get an instance
 * obj ( & recobj );	// internal modification is done.
 * obj.reconfigure ( & recobj ); //the same as above;
 */


class cReconfigurator_Interactives: public Record_Reconfigurator {
private:
	unsigned int my_index;
	vector < unsigned int > relevant_indice;
public:
	cReconfigurator_Interactives(const string & my_name, const vector <string> & relevant_attributes);
	void reconfigure ( const Record *) const;
};

/*
 * cReconfigurator_Coauthor:
 * This class is primarily used to clean the data obtained from loading from the Coauthor column.
 * Since our coauthor column is not clean enough ( with some legacy expression ), this reconfigurator discards the information from
 * the loaded coauthor column and builds a clean one.
 * Therefore, this class should be used very cautiously, as it wipes everything about coauthor WHENEVER an object of such class is created.
 *
 * Private:
 * 		const map < const Record *, cGroup_Value, cSort_by_attrib > * reference_pointer: a pointer to a patent tree, which can be obtained in a cBlocking_Operation_By_Coauthor object.
 * 		const unsigned int coauthor_index: the index of the coauthor column in many columns.
 *
 * Public:
 * 		cReconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors):
 * 			create a class object through a patent tree ( which is usually from a cBlocking_Operation_By_Coauthor object ).
 * 		void reconfigure ( const Record * ) const: virtual function.
 *
 *
 * Example:
 * list < const Record * > complete; // create a complete list of Record objects
 * Do something to fill in the list;
 * cBlocking_Operation_By_Coauthor bobcobj( complete, 1 ); // create a cBlocking_Operation_By_Coauthor object.
 * cReconfigurator_Coauthor rcobj ( bobcobj.get_patent_tree() );	//create an object of coauthor reconfigurator. This also wipes everything of the whole coauthor column!!
 * std::for_each ( complete.begin(), complete.end(), rcobj ); // this rebuilds the coauthor for each patent.
 *
 *
 */

class cReconfigurator_Coauthor : public Record_Reconfigurator {
private:
	const map < const Record *, cGroup_Value, cSort_by_attrib > * reference_pointer;
	const unsigned int coauthor_index;
public:
	cReconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors);
	void reconfigure ( const Record * ) const;
};





//===========================================================


class cRatios; //forward declaration

/*
 * disambiguate_by_set:
 * This is a global function. It takes information from two clusters ( not cluster_info !!) and return whether the two are of the same inventor,
 * and if yes, what the cohesion should be.
 *
 * Arguments:
 * 		key1: the pointer to the representative record of the first cluster
 * 		match1: the list of the pointers that belongs to the first cluster, including key1.
 * 		cohesion1: the internal cohesive value of the first cluster, interpreted as the probability for all the members of the first cluster to be of the same inventor.
 * 		key2: the pointer to the representative record of the second cluster
 * 		match2: the list of the pointers that belongs to the second cluster, including key2.
 * 		cohesion2: the internal cohesive value of the second cluster, interpreted as the probability for all the members of the second cluster to be of the same inventor.
 * 		prior: the priori probability. Check for the math equation of the way to calculate the probability.
 * 		ratio: the map (binary tree) that maps a similarity profile ( vector<unsigned int> ) to a positive real number (double).
 * 		threshold: threshold of the probability that the two clusters should be the of the same inventor.
 *
 * Return Value:
 * 		std::pair < const Record *, double >: This is a pair value, which consists of two parts.
 * 		1. const Record *: NULL if the two clusters are identified as of different inventors, and key1 if they are of the same inventors.
 * 		2. double: the cohesion of the combination of the first and the second cluster. This is only valid if the first returned pointer is not NULL.
 */

std::pair<const Record *, double> disambiguate_by_set (const Record * key1, const cGroup_Value & match1, const double cohesion1,
									 const Record * key2, const cGroup_Value & match2, const double cohesion2,
									 const double prior, 
									 const cRatios & ratio,  const double threshold ) ;

/*
 * copyfile:
 * A file copy function.
 */

void copyfile(const char * target, const char * source);
Attribute * create_attribute_instance ( const string & id );

void build_patent_tree( map < const Record *, cGroup_Value, cSort_by_attrib > & patent_tree , const list < Record > & all_records );
void build_patent_tree( map < const Record *, cGroup_Value, cSort_by_attrib > & patent_tree , const list < const Record * > & all_rec_pointers );
string check_file_existence(const string & description);

#endif /* PATENT_ENGINE_H */
