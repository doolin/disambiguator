
#include "DisambigEngine.h"
#include "attribute.h"
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

/**
 * Aim: to truncate string as desired. See the explanation in the
 * header file for more details
 * 
 * Algorithm: simple string manipulation in C.
 */

string
StringTruncate::manipulate( const string & inputstring ) const {

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
        throw StringTruncate::cException_String_Truncation(inputstring.c_str());
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



const string cBlocking_Operation::delim = "##";

/*
 * Aim: to extract initials of each word in a string, maybe not starting from the first word.
 * See the explanation in the header file for more details
 * Algorithm: simple string manipulation in C.
 */
string 
ExtractInitials::manipulate( const string & inputstring ) const {

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
StringExtractFirstWord::manipulate( const string & inputstring ) const {

    string res = inputstring.substr(0, inputstring.find(delimiter, 0));
    return res;
}


