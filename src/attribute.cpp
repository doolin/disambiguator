
#include <algorithm>
#include <cstring>

#include "attribute.h"

using std::list;
using std::string;

const SimilarityCompare::cException_Different_Similarity_Dimensions
SimilarityCompare::default_sp_exception("Error: Different Similarity profile dimensions");

vector <string> Attribute::Derived_Class_Name_Registry;


/**
 * This function splits the input string and save it into the attribute object.
 * Legacy format of data is in the form of "DATA1~COUNT1/DATA2~COUNT2/DATA3~COUNT3",
 * so this function splits the string and saves the pointer to "DATA1", 'DATA2', 'DATA3'.
 * There was a counter class member, but it was removed later.
 *
 * One needs to be very familiar with how the strings look like and what
 * they can be in the source text file. Especially the delimiters.
 * However, one is also assumed to have familiarized with the source text format.
 *
 */
bool
Attribute::split_string(const char* recdata) {

    static const string emptystring ("");
    vector < const string * > & data = this->get_data_modifiable();
    const char * p = recdata;
    const char * pend = p + strlen(recdata);

    if ( pend == p ) {
        data.push_back( this->add_string(emptystring));
        return true;
    }

    char string_count_cache[10];

    const char delim = '/';
    const char secondary_delim = '~';
    const char * q;
    uint32_t count_length;

    while ( (q = std::find(p, pend, delim)) != pend ) {
        // r points to the secondary delimiter
        // q points to the primary delimiter
        const char * r = std::find(p, q, secondary_delim);
        const string temp ( p, r );
        data.push_back(this->add_string(temp));
        if ( r != q ) {
            count_length = q - r - 1;
            memcpy(string_count_cache, r + 1, count_length *sizeof(char) );
            *(string_count_cache + count_length ) = '\0';
        }
        else {
        }
        p = q + 1;
    }

    const char * r = std::find(p, q, secondary_delim);
    const string tp ( p, r);
    const string * ptr = this->add_string(tp);
    data.push_back( ptr );

    if ( r != q ) {
        count_length = q - r - 1;
        memcpy(string_count_cache, r + 1, count_length *sizeof(char) );
        *(string_count_cache + count_length ) = '\0';
    }
    else {
    }

    // now use swap trick to minimize the volumn of
    // each attribute. Effective STL by Scott Meyers, Item 17
    vector< const string* > (data).swap(data);

    // Oh ugly, ugly code... this is not the place to
    // enforce a length constraint. And besides, why
    // is this "data" being held in a vector<string>
    // when the size is only 1? Makes no sense.
    // TODO: Get rid of this vector junk for firstnames.
    if (data.size() > 1) {
        throw cException_Vector_Data(recdata);
    }

    return true;
}


/**
 * This is a global function, not a class member function.
 * usually for set_mode classes, attrib_merge works, and
 * classes of single_mode do not support such operation.
 * if this function is called, the attributes in list1 and
 * list2 will be merged into a larger attribute object,
 * and the pointers in list1 and list2 will then point
 * to the newly created large object.
 */
void
attrib_merge (list < const Attribute * *> & l1, list < const Attribute * *> & l2) {

    static const string errmsg = "Error: attribute pointers are not pointing to the same object. Attribute Type = ";
    if ( l1.empty() || l2.empty() )
        return;

    // calls the class member function to check if attrib merge is supported.
    // if Null is returned, it means attrib_merge is not supported.
    // usually for set_mode classes, attrib_merge works, and classes of single_mode do not support such operation.
    const Attribute * new_object_pointer = (*l1.front())->attrib_merge(** l2.front());
    if ( new_object_pointer == NULL )
        return;

    for ( list < const Attribute * * >::const_iterator p = l1.begin(); p != l1.end(); ++p ) {
        if ( **p != * l1.front() ) {
            std::cout << "------------" <<std::endl;
            std::cout << "Front address: " << *l1.front() << " Other address: " << **p << std::endl;
            (*l1.front())->print();
            (**p)->print();
            std::cout << "------------" << std::endl;
            throw cException_Other ( ( errmsg + (*l1.front())->get_class_name()).c_str() ) ;
        }
    }

    for ( list < const Attribute *  * >::const_iterator p = l2.begin(); p != l2.end(); ++p ) {
        if ( **p != *l2.front() ) {
            std::cout << "------------" <<std::endl;
            std::cout << "Front address: " << *l2.front() << " Other address: " << **p << std::endl;
            (*l2.front())->print();
            (**p)->print();
            std::cout << "------------" << std::endl;
            throw cException_Other ( ( errmsg + (*l2.front())->get_class_name()).c_str() );
        }
    }


    const uint32_t l1_size = l1.size();
    const uint32_t l2_size = l2.size();

    if ( l1_size != 1 )
        (*l1.front())->reduce_attrib(l1_size - 1);
    if ( l2_size != 1)
        (*l2.front())->reduce_attrib(l2_size - 1);
    if ( l1_size + l2_size != 2 )
        new_object_pointer->add_attrib( l1_size + l2_size - 2 );

    for ( list < const Attribute * * >::const_iterator p = l1.begin(); p != l1.end(); ++p )
        **p = new_object_pointer;
    for ( list < const Attribute *  * >::const_iterator p = l2.begin(); p != l2.end(); ++p )
        **p = new_object_pointer;

}

void Attribute::register_class_names( const vector < string > & input) {
    Derived_Class_Name_Registry = input;
}

int Attribute::position_in_registry( const string & s ) {
    int i = 0;
    for ( i = 0; i < static_cast<int> (Derived_Class_Name_Registry.size()); ++i ) {
        if ( s == Derived_Class_Name_Registry.at(i))
            return i;
    }
    return -1;
}



template <> const string Attribute_Basic<cFirstname>::class_name = "Firstname";
template <> const string Attribute_Basic<cFirstname>::attrib_group = "Personal";


template <> const string Attribute_Basic<cLastname>::class_name = "Lastname";
template <> const string Attribute_Basic<cLastname>::attrib_group = "Personal";

template <> const string Attribute_Basic<cMiddlename>::class_name = "Middlename";
template <> const string Attribute_Basic<cMiddlename>::attrib_group = "Personal";

//template <> const string Attribute_Basic<cLatitude>::class_name = "Latitude";
//template <> const string Attribute_Basic<cLatitude>::interactive_column_names[] = {"Longitude", "Street", "Country"};
//template <> const uint32_t Attribute_Basic<cLatitude>::num_of_interactive_columns = 3;
//template <> const string Attribute_Basic<cLatitude>::attrib_group = "Patent";

template <> const string Attribute_Basic<cLatitude_Data >::class_name = "Latitude_Data";

template <> const string Attribute_Basic<cLatitude >::class_name = "Latitude";
template <> const string Attribute_Basic<cLatitude >::interactive_column_names[] = {"Longitude", "Street", "Country"};
template <> const uint32_t Attribute_Basic<cLatitude >::num_of_interactive_columns = 3;
template <> const string Attribute_Basic<cLatitude >::attrib_group = "Patent";

template <> const string Attribute_Basic<cLongitude_Data >::class_name = "Longitude_Data";
template <> const string Attribute_Basic<cLongitude>::class_name = "Longitude";
//template <> const string Attribute_Basic<cLongitude >::class_name = "Longitude";
template <> const string Attribute_Basic<cLongitude >::interactive_column_names[] = {"Latitude"};
template <> const uint32_t Attribute_Basic<cLongitude >::num_of_interactive_columns = 1;
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
template <> const uint32_t Attribute_Basic<cAssignee>::num_of_interactive_columns = 1;
//const map<string, std::pair<string, uint32_t>  > * cAssignee::assignee_tree_pointer;
map < const cAsgNum*, uint32_t > cAssignee:: asgnum2count_tree;
bool cAssignee::is_ready = false;
template <> const string Attribute_Basic<cAssignee>::attrib_group = "Patent";

template <> const string Attribute_Basic<cAsgNum>::class_name = "AsgNum";

template <> const string Attribute_Basic<cUnique_Record_ID>::class_name = "Unique_Record_ID";

template <> const string Attribute_Basic<cApplyYear>::class_name = "ApplyYear";
template <> const string Attribute_Basic<cAppYear>::class_name = "AppYear";
template <> const string Attribute_Basic<cGYear>::class_name = "GYear";

template <> const string Attribute_Basic<cCity>::class_name = "City";

template <> const string Attribute_Basic<cPatent>::class_name = "Patent";


/**
 * Whenever overriding a comparison fucntion, it is extremely
 * important to check if the comparator is activated. i.e.
 * Always keep the following statement in a comparison function:
 *
 * if ( ! is_comparator_activated () )
        throw cException_No_Comparision_Function(static_get_class_name().c_str());
 */

uint32_t cFirstname::previous_truncation = 0;
uint32_t cFirstname::current_truncation = 0;


/**
 * cFirstname::split_string does 3 things:
 * 1. Extract the first name from an input string, which is usually mixed with first name and middle name.
 *       i.e. Input string = "JOHN David WILLIAM", extracted string = "JOHN"
 * 2. Keep the original copy of the original input string.
 * 3. Save the first extracted string in data[0], and the second original copy in data[1].
 */
bool
cFirstname::split_string(const char *inputdata) {

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


uint32_t
cFirstname::compare(const Attribute & right_hand_side) const {

    // ALWAYS CHECK THE ACTIVITY OF COMPARISON FUNCTION !!
    if (!this->is_comparator_activated ()) {
        throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
    }

    if (this == &right_hand_side) {
        return this->get_attrib_max_value();
    }

    uint32_t res = 0;
    res = name_compare(* this->get_data().at(1), *right_hand_side.get_data().at(1),
        previous_truncation, current_truncation);

    if (res > this->get_attrib_max_value()) {
        res = this->get_attrib_max_value();
    }

    return res;
}



/**
 * cMiddlename::split_string does 3 things:
 * 1. Extract the middle name from an input string, which is usually mixed with first name and middle name.
 *       i.e. Input string = "JOHN David WILLIAM", extracted string = "David WILLIAM"
 *            Input string = "JOHN", extracted string = ""
 *
 * 2. Extract the initials of middle names.
 *       i.e. Input string = "JOHN DAVID WILLIAM", middle initials = "DW"
 * 3. Save the first extracted string in data[0], and the second middle initial string in data[1].
 */
bool
cMiddlename::split_string(const char *inputdata) {

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


/**
 * cMiddlename::compare:
 * Compare the extracted strings in data[0] to see whether they
 * started with the same letter and whether one contains the other.
 * i.e.
 * "DAVID WILLIAM" vs "DAVID" = 3 ( max score)
 * "DAVID WILLIAM" vs "WILLIAM" = 0 (min score, not starting with the same letters)
 * "DAVID WILLIAM" vs "DAVE" = 0 ( either one does not container the other. )
 * "" vs "" = 2 ( both missing information )
 * "DAVID" vs "" = 1 ( one missing information )
 */
uint32_t
cMiddlename::compare(const Attribute & right_hand_side) const {

    if (!is_comparator_activated()) {
        throw cException_No_Comparision_Function(static_get_class_name().c_str());
    }

    try {

        const cMiddlename & rhs = dynamic_cast<const cMiddlename &> (right_hand_side);
        uint32_t res = midnamecmp(* this->get_data().at(0), *rhs.get_data().at(0));
        if (res > max_value) res = max_value;
        return res;

    } catch ( const std::bad_cast & except ) {

        std::cerr << except.what() << std::endl;
        std::cerr << "Error: " << this->get_class_name() << " is compared to "
                  << right_hand_side.get_class_name() << std::endl;
        throw;
    }
}



/**
 * cLatitude::compare:
 *
 * Such comparison is complicated because cLatitude is
 * interacted with cLongitude, cCountry , and possibly cStreet
 *
 * One needs to know the structure of how the interactive data
 * are stored, which is actually an assumed knowledge.
 *
 * If the distance calculated by latitude and longitude < 1 mile, score = 4 ( max score)
 * If the distance calculated by latitude and longitude < 10 mile, score = 3
 * If the distance calculated by latitude and longitude < 50 mile, score = 2
 * If the distance calculated by latitude and longitude > 50 mile, score = 1
 * If countries are different, score = 0;
 */
uint32_t
cLatitude::compare(const Attribute & right_hand_side) const {

    if (!is_comparator_activated())
        throw cException_No_Comparision_Function(static_get_class_name().c_str());

    check_if_reconfigured();

#define LATCOMPS 0
#if LATCOMPS

    if (this->is_informative()) {
      std::cout << "is informative\n";
    } else {
      std::cout << "is not informative\n";
    }

    if (this->exact_compare(right_hand_side)) {
      std::cout << "compares exactly\n";
    } else {
      std::cout << "does not compare exactly \n";
    }
#endif


    try {
        uint32_t res = 0;
        const cLatitude & rhs = dynamic_cast< const cLatitude & > (right_hand_side);

        const Attribute* const & this_longitude = this->get_interactive_vector().at(0);
        const Attribute* const & rhs_longitude = rhs.get_interactive_vector().at(0);

        if (this->get_data().size() != this_longitude->get_data().size()) {
            std::cout << "Alignment error in latitude comparison: " << std::endl;
            this->print(std::cout);
            this_longitude->print(std::cout);
            throw cException_Interactive_Misalignment(this->get_class_name().c_str());
        }

        if (rhs.get_data().size() != rhs_longitude->get_data().size()) {
            std::cout << "Alignment error in latitude comparison: " << std::endl;
            rhs.print(std::cout);
            rhs_longitude->print(std::cout);
            throw cException_Interactive_Misalignment(this->get_class_name().c_str());
        }

        // Latitude interacts with {"Longitude", "Street", "Country"}; the sequence is important.

        uint32_t country_score = 0;
        if (this == &rhs && this->is_informative()) {
            res = max_value;
        } else {

            // Comparing country
            const Attribute * country1 = this->get_interactive_vector().at(2);
            const Attribute * country2 = rhs.get_interactive_vector().at(2);
            //if (this->get_interactive_vector().at(2) == rhs.get_interactive_vector().at(2)) {
            if (country1->get_data() == country2->get_data()) {
                country_score = 1;
            }

            // Comparing street;
            //uint32_t street_score = 0;

            // Comparing Latitidue and longitude

            uint32_t latlon_score = 0;
            latlon_score = latloncmp ( * this->get_data().at(0), * this_longitude->get_data().at(0),
                                        * rhs.get_data().at(0), * rhs_longitude->get_data().at(0) );

            if (country_score == 0) {
                res = 0;
            } else {
                res = latlon_score;
            }
        }

        //correction for japanese
        if (country_score == 1 && *this->get_interactive_vector().at(2)->get_data().at(0) == "JP") {
            const Attribute* const & this_street = this->get_interactive_vector().at(1);
            const Attribute* const & rhs_street = rhs.get_interactive_vector().at(1);
            if ( this_street == rhs_street && ( ! this_street->is_informative() ) )
                res -= 1;
        }

        if ( res > max_value )
            throw cException_Other("latitude error: score > max_value");

        return res;

    } catch ( const std::bad_cast & except ) {
        std::cerr << except.what() << std::endl;
        std::cerr << "Error: " << this->get_class_name()
                  << " is compared to " << right_hand_side.get_class_name()
                  << std::endl;
        throw;
    }
}


uint32_t
cLongitude::compare(const Attribute & right_hand_side) const {

    if (!is_comparator_activated())
        throw cException_No_Comparision_Function(static_get_class_name().c_str());

    // ~L1412 in attribute.h
    check_if_reconfigured();

#define LONGCOMPS 0
#if LONGCOMPS
    if (this->is_informative()) {
      std::cout << "is informative\n";
    } else {
      std::cout << "is not informative\n";
    }

    if (this->exact_compare(right_hand_side)) {
      std::cout << "compares exactly\n";
    } else {
      std::cout << "does not compare exactly \n";
    }
#endif

    try {
        uint32_t res = 0;
        const bool exact_same = this->exact_compare(right_hand_side) == 1 ;

        if (exact_same && this->is_informative()) res = 1;
        if (res > max_value) res = max_value;
        return res;

    } catch (const std::bad_cast & except) {
        std::cerr << except.what() << std::endl;
        std::cerr << "Error: " << this->get_class_name()
                  << " is compared to " << right_hand_side.get_class_name()
                  << std::endl;
        throw;
    }
}


/**
 * cClass_M2::compare
 * A second way to score the "class" attribute.
 * Not in use now.
 */
uint32_t
cClass_M2::compare(const Attribute & right_hand_side) const {

    const cClass_M2 & rhs = dynamic_cast< const cClass_M2 & > (right_hand_side);
    const uint32_t common = this->Attribute_Set_Mode <cClass_M2>::compare( rhs );
    const uint32_t this_size = this->attrib_set.size();
    const uint32_t rhs_size = rhs.attrib_set.size();

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


/**
 * cCountry::compare
 * Not supposed to be used, because country attribute is mixed in the latitude comparison.
 *
 */
uint32_t
cCountry::compare(const Attribute & right_hand_side) const {

    if ( ! is_comparator_activated () )
        throw cException_No_Comparision_Function(static_get_class_name().c_str());

    if ( !this->is_informative() || ! right_hand_side.is_informative() )
        return 1;
    if ( this == & right_hand_side )
        return 2;
    else
        return 0;
}



/**
 * cAssignee::compare:
 *
 * Comparison of assignee includes two steps:
 *
 * 1. look up the assignee->asgnum(assignee number) tree,
 *    in order to check whether two assignees shared the same number.
 *    If they do, they are believed to be the same, and the score
 *    of 5 is granted. If the assignee has fewer than 100 different
 *    patents, an additional 1 point is added.
 *
 * 2. If their assignee numbers are different, they can still be the same assignee.
 *
 *    (OBSOLETE, DOESN'T MATCH CODE)
 *    In this case, a fairly crude jaro-winkler string comparison is used to score.
 *    Refer to the function jwcmp for more scoring information.
 *
 */
uint32_t
cAssignee::compare(const Attribute & right_hand_side) const {

    if (!is_comparator_activated())
        throw cException_No_Comparision_Function(static_get_class_name().c_str());

    // TODO: figure out where configure_assignee is invoked
    if (!cAssignee::is_ready)
        throw cException_Other("Trees for assignee comparison are not set up yet. Run cAssignee::configure_assignee first.");

    try {

        const cAssignee & rhs = dynamic_cast< const cAssignee & > (right_hand_side);

        //uint32_t res = asgcmp(this->get_data(), rhs.get_data(), assignee_tree_pointer);
        //uint32_t res = asgcmp ( * this->get_data().at(0), * rhs.get_data().at(0), assignee_tree_pointer);

        uint32_t res = 0;
        const cAsgNum * p = dynamic_cast<const cAsgNum *>(this->get_interactive_vector().at(0));

        if (!p)
            throw cException_Other("Cannot dynamic cast to cAsgNum *.");

        const cAsgNum * q = dynamic_cast < const cAsgNum *> (rhs.get_interactive_vector().at(0));
        if (!q)
            throw cException_Other("Cannot dynamic cast rhs to cAsgNum *.");

        if (!this->is_informative() || !rhs.is_informative()) {
            res = 1;
        }
        else if (p != q) {
            res = asgcmp(* this->get_data().at(0), * rhs.get_data().at(0));
        } else {

            res = 5;
            map<const cAsgNum *, uint32_t>::const_iterator t = cAssignee::asgnum2count_tree.find(p);

            if (t == cAssignee::asgnum2count_tree.end())
                throw cException_Other("AsgNum pointer is not in tree.");

            if (t->second < 100)
                ++res;
        }

        if (res > max_value)
            res = max_value;

        return res;
    }
    catch (const std::bad_cast & except) {
        std::cerr << except.what() << std::endl;
        std::cerr << "Error: " << this->get_class_name() << " is compared to "
                  << right_hand_side.get_class_name() << std::endl;
        throw;
    }
}


/**
 * The reason to override cCity::split_string from its base is because
 * a city's name can include some delimiters in the default base implementation.
 */
bool
cCity::split_string(const char* source) {

    string temp (source);
    get_data_modifiable().push_back(this->add_string(temp));
    return true;
}
