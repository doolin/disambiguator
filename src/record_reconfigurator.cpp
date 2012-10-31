

#include "engine.h"

/**
 * Aim: constructor of Reconfigurator_AsianNames.
 */
Reconfigurator_AsianNames::Reconfigurator_AsianNames(): country_index(Record::get_index_by_name(cCountry::static_get_class_name())),
                                firstname_index(Record::get_index_by_name(cFirstname::static_get_class_name())),
                                middlename_index(Record::get_index_by_name(cMiddlename::static_get_class_name())),
                                lastname_index(Record::get_index_by_name(cLastname::static_get_class_name())){
    east_asian.push_back(string("KR"));
    east_asian.push_back(string("CN"));
    east_asian.push_back(string("TW"));
}


/**
 * Aim: reconfigure the asian name in accordance with the current scoring system.
 *
 * Algorithm: check the country first. If the country is in [ "KR", "CN", "TW"],
 * do this operation:
 *  1. Create a vector of string, and save FULL first name into the vector. 
 *     Then create a firstname attribute from the vector
 *  2. Create a vector of string, and save "FULL firstname" + "FULL lastname"
 *     into the vector. Then create a middlename attribute from the vector.
 *  3. Change the Record data such that the firstname pointer points to the
 *     newly created firstname object. So for the middle name.
 */
void
Reconfigurator_AsianNames::reconfigure( const Record * p ) const {

    bool need_reconfigure = false;
    const string & country = *  p->get_attrib_pointer_by_index(country_index)->get_data().at(0) ;

    for ( register vector<string>::const_iterator ci = east_asian.begin(); ci != east_asian.end(); ++ci )
        if ( country == *ci ) {
            need_reconfigure = true;
            break;
        }

    if (need_reconfigure == false) return;

    // do not change original attributes. add new ones.
    const Attribute * cur_af = p->get_attrib_pointer_by_index(firstname_index);
    const string & fn_alias = * cur_af ->get_data().at(0);
    const vector <string> fn ( 2, fn_alias );
    const Attribute * paf = cFirstname::static_clone_by_data(fn);

    const Attribute * cur_am = p->get_attrib_pointer_by_index(middlename_index);
    const string & lnstr = * p->get_attrib_pointer_by_index(lastname_index)->get_data().at(0);
    const string mnstr ( fn_alias + "." + lnstr);
    const vector < string > mn(2, mnstr);
    const Attribute * pam = cMiddlename ::static_clone_by_data(mn);

    Record * q = const_cast < Record * > (p);

    cur_af->reduce_attrib(1);
    cur_am->reduce_attrib(1);
    q->set_attrib_pointer_by_index(paf, firstname_index);
    q->set_attrib_pointer_by_index(pam, middlename_index);
}


Reconfigurator_Interactives::Reconfigurator_Interactives(const string & my_name,
        const vector<string > & relevant_attribs) {

    my_index = Record::get_index_by_name(my_name);

    vector<string>::const_iterator p = relevant_attribs.begin();
    for (;  p != relevant_attribs.end(); ++p) {
        unsigned int idx = Record::get_index_by_name(*p);
        relevant_indice.push_back(idx);
    }
}


void
Reconfigurator_Interactives::reconfigure ( const Record * p ) const {

    vector < const Attribute * > interact;
    for ( vector < unsigned int >::const_iterator i = relevant_indice.begin(); i != relevant_indice.end(); ++i ) {
        interact.push_back(p->get_attrib_pointer_by_index(*i));
    }
    const Attribute * const & tp = p->get_attrib_pointer_by_index(my_index);
    const Attribute * & cp = const_cast< const Attribute * &> (tp);
    cp = tp->config_interactive(interact);
}


/**
 * Aim: constructor of Reconfigurator_Coauthor
 *
 * Algorithm: The constructor calls cCoauthor::clear_data_pool() and
 * cCoauthor::clear_attrib_pool(). So it is critical to remember that
 * construction of each Reconfigurator_Coauthor object will DESTROY
 * all the information of any cCoauthor class and INVALIDATE any pointers
 * that point to a cCoauthor object. Therefore, creation of
 * Reconfigurator_Coauthor objects shall NEVER happen during disambiguation.
 *
 */
Reconfigurator_Coauthor::Reconfigurator_Coauthor(
    const map < const Record *, RecordPList, cSort_by_attrib > & patent_authors)
    : reference_pointer (&patent_authors),
      coauthor_index (Record::get_index_by_name(cCoauthor::static_get_class_name())) {

    cCoauthor::clear_data_pool();
    cCoauthor::clear_attrib_pool();
}


/**
 * Aim: to recreate the whole coauthor database, and link them to appropriate pointers.
 *
 * Algorithm: for each unique record id, find all other unique id records
 * with which this one is associated. Then extract the FULL names
 * of those records, and save in cCoauthor data pool. After that,
 * reset the pointer of this unique record to point to the newly built
 * attribute.
 */
void
Reconfigurator_Coauthor::reconfigure(const Record * p) const {

    static const string dot = ".";
    static const unsigned int firstnameindex = Record::get_index_by_name(cFirstname::static_get_class_name());
    static const unsigned int lastnameindex = Record::get_index_by_name(cLastname::static_get_class_name());
    static const StringExtractFirstWord firstname_extracter;
    static const StringRemoveSpace lastname_extracter;

    map<const Record *, RecordPList, cSort_by_attrib>::const_iterator cpm;
    cCoauthor temp;

    cpm = reference_pointer->find(p);

    if (cpm == reference_pointer->end())
        throw cException_Other("Missing patent data.");

    const RecordPList & patent_coauthors = cpm->second;

    RecordPList::const_iterator q = patent_coauthors.begin();
    for (; q != patent_coauthors.end(); ++q ) {

        if (*q == p)
            continue;

        string fullname = firstname_extracter.manipulate( * (*q)->get_data_by_index(firstnameindex).at(0) ) + dot
                            + lastname_extracter.manipulate( * (*q)->get_data_by_index(lastnameindex).at(0) );
        temp.attrib_set.insert(cCoauthor::static_add_string (fullname) );
    }

    const Attribute * np = cCoauthor::static_add_attrib(temp, 1);
    const Attribute ** to_change = const_cast< const Attribute **> ( & p->get_attrib_pointer_by_index(coauthor_index));
    *to_change = np;

}


const
Record_Reconfigurator * generate_interactive_reconfigurator( const Attribute * pAttrib) {

    vector <string > linked_attribs (pAttrib->get_interactive_class_names());
    string my_name = pAttrib->get_class_name();
    //ATTENTION: OBJECT IS ON HEAP.
    const Record_Reconfigurator * preconfig = new Reconfigurator_Interactives( my_name, linked_attribs );
    return preconfig;
}

