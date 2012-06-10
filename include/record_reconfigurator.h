

#include "record.h"


/**
 * Record_Reconfigurator
 *     - Reconfigurator_AsianNames
 *     - Reconfigurator_Latitude_Interactives
 *     - Reconfigurator_Coauthor
 *
 * The record reconfigurator hierarchy is used to modify
 * the data after initial loading from text files.
 * For some reasons, initial data are not ideal for disambiguation,
 * and modification of such data is necessary.
 * It is also critical to reconfigure Record objects when
 * interactions between attributes exist.
 * To add more concrete classes, simply inherit the Record_Reconfigurator
 * class and implement the "void reconfigure ( const Record * ) const" virtual function.
 * To use a concrete Record_Reconfigurator class, create an object of
 * the class and treat the object as a function, as shown below.
 *
 */
class Record_Reconfigurator {
public:
    virtual void reconfigure ( const Record * ) const = 0;
    void operator () ( Record & r ) const  {this->reconfigure(&r);}
    void operator () ( const Record * p) const { this->reconfigure(p); }
    virtual ~Record_Reconfigurator(){}
};


/**
 * Reconfigurator_AsianNames:
 * This class is specifically designed for east asian names. Generally,
 * east asian names do not have middle names. For example,
 * "MING LIANG.ZHANG" of China has the first name as "MING LIANG"
 * instead of "MING", whereas the originally regarded "LIANG" as
 * middle name actually is not middle name. Moreover, their first
 * names and last names are usually extremely similar in terms of spelling.
 * Therefore, to accommodate reasonable comparisons, their first names are
 * set to be the original strings from the raw text data,
 * and their middle names are set to be the combination of first
 * names and last names.
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
 *         Reconfigurator_AsianNames(): constructor
 *         void reconfigure( const Record * ) const: virtual function.
 *
 *
 * For example: assuming there is a record object, recobj, which has the
 * cFirstname attribute, cMiddlename and cLastname attribute shown as below.
 *
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
 *  Reconfigurator_AsianNames rcfgobj; //create a reconfigurator object.
 *  rcfgobj ( & recobj ); // rcfgobj is a Reconfigurator_AsianNames object
 *  We now have:
 *
 *  After reconfiguration:
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
class Reconfigurator_AsianNames : public Record_Reconfigurator {

private:
    const unsigned int country_index;
    const unsigned int firstname_index;
    const unsigned int middlename_index;
    const unsigned int lastname_index;
    vector < string > east_asian;
    const StringRemoveSpace rmvsp;

public:
    Reconfigurator_AsianNames();
    void reconfigure( const Record * ) const;
};



/**
 * Reconfigurator_Interactives:
 * This class is used to modify the objects to connect with its associated other Attribute objects.
 *
 * Example:
 * vector < string > links;
 * links.push_back("Longitude");
 * links.push_back("Street");
 * links.push_back("Country");
 * Reconfigurator_Interactives obj ("Latitude", links);    // get an instance
 * obj ( & recobj );    // internal modification is done.
 * obj.reconfigure ( & recobj ); //the same as above;
 */
class Reconfigurator_Interactives: public Record_Reconfigurator {

private:
    unsigned int my_index;
    vector < unsigned int > relevant_indice;
public:
    Reconfigurator_Interactives(const string & my_name, const vector <string> & relevant_attributes);
    void reconfigure ( const Record *) const;
};


/**
 * Reconfigurator_Coauthor:
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
 *         Reconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors):
 *             create a class object through a patent tree ( which is usually from a cBlocking_Operation_By_Coauthor object ).
 *         void reconfigure ( const Record * ) const: virtual function.
 *
 *
 * Example:
 * list < const Record * > complete; // create a complete list of Record objects
 * Do something to fill in the list;
 * cBlocking_Operation_By_Coauthor bobcobj( complete, 1 ); // create a cBlocking_Operation_By_Coauthor object.
 * Reconfigurator_Coauthor rcobj ( bobcobj.get_patent_tree() );    //create an object of coauthor reconfigurator. This also wipes everything of the whole coauthor column!!
 * std::for_each ( complete.begin(), complete.end(), rcobj ); // this rebuilds the coauthor for each patent.
 *
 *
 */
class Reconfigurator_Coauthor : public Record_Reconfigurator {

private:
    const map < const Record *, cGroup_Value, cSort_by_attrib > * reference_pointer;
    const unsigned int coauthor_index;
public:
    Reconfigurator_Coauthor ( const map < const Record *, cGroup_Value, cSort_by_attrib > & patent_authors);
    void reconfigure ( const Record * ) const;
};


