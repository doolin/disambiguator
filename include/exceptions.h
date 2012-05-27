
#ifndef PATENT_EXCEPTIONS_H
#define PATENT_EXCEPTIONS_H

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



/*
 *  cAbstract_Exception
 *  	-- cException_Diff_Attribute
 *  	-- cException_No_Comparision_Function
 *  	-- cException_ColumnName_Not_Found
 *  	-- cException_Insufficient_Interactives
 *  	-- cException_No_Interactives
 *  	-- cException_Invalid_Function
 *  	-- cException_Interactive_Misalignment
 *  	-- cException_File_Not_Found
 *  	-- cException_Assignee_Not_In_Tree
 *  	-- cException_Invalid_Attribute_For_Sort
 *  	-- cException_Invalid_Probability
 *  	-- cException_Vector_Data
 *  	-- cException_Attribute_Not_In_Tree
 *  	-- cException_Duplicate_Attribute_In_Tree
 *  	-- cException_Unknown_Similarity_Profile
 *  	-- cException_Attribute_Disabled
 *  	-- cException_Other
 *  	-- cException_Blocking_Disabled
 *
 *  To create a new exception class, simply inherit the cAbstract_Exception class, and write a constructor.
 *
 */


class cAbstract_Exception: public std::exception {
private:
	string m_errmsg;
public:
	cAbstract_Exception(const char *errmsg) : m_errmsg(errmsg) {};
	const char * what() const throw() {return m_errmsg.c_str();}
	~cAbstract_Exception() throw() {};
};


class cException_Diff_Attribute : public cAbstract_Exception{};


class cException_No_Comparision_Function : public cAbstract_Exception {
public:
	cException_No_Comparision_Function(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_ColumnName_Not_Found : public cAbstract_Exception {
public:
	cException_ColumnName_Not_Found(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Insufficient_Interactives: public cAbstract_Exception {
public:
	cException_Insufficient_Interactives(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_No_Interactives: public cAbstract_Exception {
public:
	cException_No_Interactives(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Invalid_Function : public cAbstract_Exception {
public:
	cException_Invalid_Function( const char * errmsg) : cAbstract_Exception(errmsg) {};
};


class cException_Interactive_Misalignment: public cAbstract_Exception {
public:
	cException_Interactive_Misalignment (const char* errmsg) : cAbstract_Exception(errmsg) {};
};


class cException_File_Not_Found: public cAbstract_Exception {
public:
	cException_File_Not_Found(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Assignee_Not_In_Tree: public cAbstract_Exception {
public:
	cException_Assignee_Not_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Invalid_Attribute_For_Sort: public cAbstract_Exception{
public:
	cException_Invalid_Attribute_For_Sort(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Invalid_Probability: public cAbstract_Exception {
public:
	cException_Invalid_Probability(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Vector_Data: public cAbstract_Exception {
public:
	cException_Vector_Data(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Attribute_Not_In_Tree: public cAbstract_Exception {
public:
	cException_Attribute_Not_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Duplicate_Attribute_In_Tree: public cAbstract_Exception {
public:
	cException_Duplicate_Attribute_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Unknown_Similarity_Profile: public cAbstract_Exception {
public:
	cException_Unknown_Similarity_Profile(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Attribute_Disabled : public cAbstract_Exception {
public:
	cException_Attribute_Disabled(const char* errmsg): cAbstract_Exception(errmsg){};
};


class cException_Other: public cAbstract_Exception {
public:
	cException_Other(const char* errmsg): cAbstract_Exception(errmsg){};
};


class cException_Blocking_Disabled : public cAbstract_Exception {
public:
	cException_Blocking_Disabled(const char* errmsg): cAbstract_Exception(errmsg){};
};

#endif /* PATENT_EXCEPTIONS_H */
