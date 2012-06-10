
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
 *
 * @param bool operator () (const Record * prec1, const Record *prec2 ) const:
 *    comparison between two records pointers based on their content.
 *       The conparison content depends on the internal information of the
 *       cSort_by_attrib object. i.e. attrib_index;
 *
 * Example:
 * 1. cSort_by_attrib obj1( "Patent" ); this will create an instance obj1
 *    which compares two Record pointers based on the "Patent" information column.
 *    map < const Record *, VALUE_TYPE, cSort_by_attrib > m1(obj1); will
 *    create a map (binary tree) whose key is const Record pointer, and
 *       the way of sorting is through comparison of "Patent" attribute.
 *       Binary tree is ideal for fast search, insertion and deletion.
 *
 */

class cSort_by_attrib {
private:
  //attrib_index is the column index on which the Record's comparison depends.
    const unsigned int attrib_index;
public:
    bool operator () (const Record * prec1, const Record *prec2 ) const {
        const Attribute * attr1 = prec1->vector_pdata.at(attrib_index);
        const Attribute * attr2 = prec2->vector_pdata.at(attrib_index);

        return attr1->get_data().at(0) < attr2->get_data().at(0);
    };
    cSort_by_attrib(const unsigned int i): attrib_index(i) {};
    cSort_by_attrib(const string & attrib_name): attrib_index(Record::get_index_by_name(attrib_name) ) {};
};


#include "string_manipulator.h"

#include "blocking_operation.h"


// Record_Reconfigurator classes

/*
 * Record_Reconfigurator
 *     - cReconfigurator_AsianNames
 *     - cReconfigurator_Latitude_Interactives
 *     - cReconfigurator_Coauthor
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
 *         const unsigned int country_index: index of country column in the record vector
 *         const unsigned int firstname_index: index of first name column in the record vector
 *         const unsigned int middlename_index: index of middle name column in the record vector
 *         const unsigned int lastname_index: index of last name column in the record vector
 *         vector < string > east_asian: string of east asian country ids.
 *         const StringRemoveSpace rmvsp: a sring manipulator. Perhaps not used here.
 *
 * Public:
 *         cReconfigurator_AsianNames(): constructor
 *         void reconfigure( const Record * ) const: virtual function.
 *
 *
 * For example: assuming there is a record object, recobj, which has the cFirstname attribute, cMiddlename and cLastname attribute shown as below.
 * Before reconfiguration:
 *  1. const Attribute * pfirstname: pointing to an attribute whose data vector contains 2 string pointers:
 *         1.1 pfirstname->data[0] points to "MING LIANG"
 *         1.2 pfirstname->data[1] points to "MING"
 *
 *  2. const Attribute * pmiddlename: pointing to an attribute whose data vector contains 2 string pointers:
 *         2.1 pfirstname->data[0] points to "MING LIANG"
 *         2.2 pfirstname->data[1] points to "LIANG"
 *
 *  3. const Attribute * plasname: pointing to an attribute whose data vector contains 2 string pointers:
 *         3.1 pfirstname->data[0] points to "ZHANG"
 *         3.2 pfirstname->data[1] points to "ZHANG"
 *
 *  By applying the reconfiguration through:
 *  cReconfigurator_AsianNames rcfgobj; //create a reconfigurator object.
 *  rcfgobj ( & recobj ); // rcfgobj is a cReconfigurator_AsianNames object
 *  We now have:
 *
 *     After reconfiguration:
 *  1. const Attribute * pfirstname: pointing to an attribute whose data vector contains 2 string pointers:
 *         1.1 pfirstname->data[0] points to "MING LIANG"
 *         1.2 pfirstname->data[1] points to "MING LIANG"
 *
 *  2. const Attribute * pmiddlename: pointing to an attribute whose data vector contains 2 string pointers:
 *         2.1 pfirstname->data[0] points to "MING LIANG.ZHANG"
 *         2.2 pfirstname->data[1] points to "MING LIANG.ZHANG"
 *
 *  3. const Attribute * plasname: pointing to an attribute whose data vector contains 2 string pointers:
 *         3.1 pfirstname->data[0] points to "ZHANG"
 *         3.2 pfirstname->data[1] points to "ZHANG"
 *
 */


class cReconfigurator_AsianNames : public Record_Reconfigurator {
private:
    const unsigned int country_index;
    const unsigned int firstname_index;
    const unsigned int middlename_index;
    const unsigned int lastname_index;
    vector < string > east_asian;
    const StringRemoveSpace rmvsp;
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
 * cReconfigurator_Interactives obj ("Latitude", links);    // get an instance
 * obj ( & recobj );    // internal modification is done.
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
 *         const map < const Record *, cGroup_Value, cSort_by_attrib > * reference_pointer: a pointer to a patent tree, which can be obtained in a cBlocking_Operation_By_Coauthor object.
 *         const unsigned int coauthor_index: the index of the coauthor column in many columns.
 *
 * Public:
 *         cReconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors):
 *             create a class object through a patent tree ( which is usually from a cBlocking_Operation_By_Coauthor object ).
 *         void reconfigure ( const Record * ) const: virtual function.
 *
 *
 * Example:
 * list < const Record * > complete; // create a complete list of Record objects
 * Do something to fill in the list;
 * cBlocking_Operation_By_Coauthor bobcobj( complete, 1 ); // create a cBlocking_Operation_By_Coauthor object.
 * cReconfigurator_Coauthor rcobj ( bobcobj.get_patent_tree() );    //create an object of coauthor reconfigurator. This also wipes everything of the whole coauthor column!!
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
 *         key1: the pointer to the representative record of the first cluster
 *         match1: the list of the pointers that belongs to the first cluster, including key1.
 *         cohesion1: the internal cohesive value of the first cluster, interpreted as the probability for all the members of the first cluster to be of the same inventor.
 *         key2: the pointer to the representative record of the second cluster
 *         match2: the list of the pointers that belongs to the second cluster, including key2.
 *         cohesion2: the internal cohesive value of the second cluster, interpreted as the probability for all the members of the second cluster to be of the same inventor.
 *         prior: the priori probability. Check for the math equation of the way to calculate the probability.
 *         ratio: the map (binary tree) that maps a similarity profile ( vector<unsigned int> ) to a positive real number (double).
 *         threshold: threshold of the probability that the two clusters should be the of the same inventor.
 *
 * Return Value:
 *         std::pair < const Record *, double >: This is a pair value, which consists of two parts.
 *         1. const Record *: NULL if the two clusters are identified as of different inventors, and key1 if they are of the same inventors.
 *         2. double: the cohesion of the combination of the first and the second cluster. This is only valid if the first returned pointer is not NULL.
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
