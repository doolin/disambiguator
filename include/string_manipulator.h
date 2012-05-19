
#ifndef PATENT_STRING_MANIPULATOR_H
#define PATENT_STRING_MANIPULATOR_H

#include <string>

/*
 * StringManipulator:
 *     - StringRemainSame
 *     - cString_Remove_Space
 *     - cString_Truncate
 *         -- cString_NoSpace_Truncate
 *     - cExtract_Initials
 *     - cString_Extract_FirstWord
 *
 * StringManipulator is a hierarchy of of string operation functors.
 * The reason to create such hierarchy is to allow polymorphism.
 * Subclasses override the virtual function
 * "string manipulate ( const string & input ) const " with their
 * own implementations.
 *
 * To use, one should create an object of a subclass, and call
 * the "manipulate" function.
 * To add more user defined string manipulators, one should simply
 * create a class that inherits the StringManipulator class,
 * and override the manipulate function.
 *
 */

class StringManipulator{
private:

public:
    // Not the right way to do this. See, for example:
    // http://steve-yegge.blogspot.com/2006/03/execution-in-kingdom-of-nouns.html
    // Substitute "manipulate" for "execute" or "run" and there you have it.
    virtual string manipulate(const string & inputstring) const = 0;
    virtual ~StringManipulator() {};
    virtual StringManipulator * clone () const = 0;
};

/*
 * StringRemainSame:
 * As is indicated by the name, this class returns the raw string it obtains.
 *
 */

class StringRemainSame : public StringManipulator {
private:
    StringManipulator * clone () const { return new StringRemainSame(*this);}
public:
    string manipulate(const string & inputstring ) const { return inputstring;}

};


/*
 * cString_Remove_Space:
 * This class remove all the white spaces of the input string and returns a cleaned one.
 * i.e. Input = "THIS IS AN   EXAMPLE  ". Return value = "THISISANEXAMPLE".
 *
 */

class cString_Remove_Space : public StringManipulator {
private:
    static const char delimiter = ' ';
    StringManipulator * clone () const { return new cString_Remove_Space(*this);}
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
 * This class is more often used for string operation.
 * Understanding of the member is desirable.
 *
 * Private:
 * int begin: location of the starting point for truncation.
 * Non-negative value means the position of starting character is from the beginning,
 * and negative value means the position of starting character is from the end of the string.
 * unsigned int nchar: number of characters to extract. if nchar == 0,
 * all the string will be extracted. If nchar > length of string, only the string part will be kept.
 * bool is_forward: indicating the direction to truncation. true = forward ( left to right ),
 * false = backward ( right to left )
 * bool is_usable: internal data of the class. false = the object is not
 * prepared yet. true = ready to use.
 *
 */

/*
 * WARNING: A COMBINATION OF NCHAR = 0 AND IS_FORWARD = FALSE WILL RETURN AN EMPTY STRING.
 *
 *
 * Public:
 *     explicit cString_Truncate(): default constructor.
 *  void set_truncater( const int inputbegin, const unsigned int inputnchar,
 * const bool inputforward): configure the object.
 *  string manipulate( const string & inputstring ) const:
 * implementation of the extraction operation. Defined in cpp file.
 *
 * Example:
 *  cString_Truncate stobj; //created an instance
 *  stobj.set_truncater(0, 5, true) : starting position = 0
 * (head of the string), extraction length = 5, direction = forward.
 *  stobj.manipulate ("ERIC") returns "ERIC".
 *  stobj.manipulate ("JOHNSON") returns "JOHNS";
 *  stobj.set_truncater(0, 0, true) : starting position = 0 (head of the string), 
         extraction length = full length, direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "JOHNSON";


 *  stobj.set_truncater(-6, 2, true) : starting position = -6, extraction length = 2,
 * direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "OH";

 *  stobj.set_truncater(-6, 2, false) : starting position = -6, extraction length = 2,
 * direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "HO";

 *  stobj.set_truncater(4, 3, false) : starting position = 4, extraction length = 4,
 * direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "SNH";

 *  stobj.set_truncater(4, 0, false) : starting position = 4, extraction length = 0,
 * direction = backward.  --- read the Warning part.
 *  stobj.manipulate ("JOHNSON") returns "";
 *
 */


class cString_Truncate: public StringManipulator {
private:
    int begin;
    unsigned int nchar;
    bool is_forward;
    bool is_usable;

    class cException_String_Truncation: public cAbstract_Exception {
    public:
        cException_String_Truncation (const char * errmsg) : cAbstract_Exception(errmsg) {};
    };
    StringManipulator * clone () const { return new cString_Truncate(*this);}
public:
    explicit cString_Truncate(): is_usable (false) {};
    void set_truncater( const int inputbegin, const unsigned int inputnchar,
                        const bool inputforward) {
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
    StringManipulator * clone () const { return new cString_NoSpace_Truncate(*this);}
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
 * cExtract_Initials eiobj(3);    //create an instance eiobj, setting the starting_word = 3;
 * eiobj.manipulate("THIS IS AN EXAMPLE, YOU KNOW.") returns "EYK".
 *
 */

class cExtract_Initials : public StringManipulator {
private:
    const unsigned int starting_word;
    static const char delimiter = ' ';
    StringManipulator * clone () const { return new cExtract_Initials(*this);}
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

class cString_Extract_FirstWord : public StringManipulator {
private:
    static const char delimiter = ' ';
    StringManipulator * clone () const { return new cString_Extract_FirstWord(*this);}
public:
    string manipulate (const string & inputstring ) const;
};


#endif /* PATENT_STRING_MANIPULATOR_H */
