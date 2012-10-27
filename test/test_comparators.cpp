
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>

#include <comparators.h>

#include "testutils.h"

using std::string;

class ComparatorsTest : public CppUnit::TestCase {

public:

  ComparatorsTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }


 /**
  * This is insane to have a distance measure
  * which returns anything other than zero for
  * zero distance. See comments in source code.
  */
  void test_zero() {
    string lat1("0.0");
    string lon1("0.0");
    string lat2("0.0");
    string lon2("0.0");
    int distance = latloncmp(lat1, lon1, lat2, lon2);
    CPPUNIT_ASSERT(distance == 1);
  }

  void test_latloncmp() {
    string lat1("38.38");
    string lon1("102.102");
    string lat2("38.38");
    string lon2("102.102");
    int distance = latloncmp(lat1, lon1, lat2, lon2);
    CPPUNIT_ASSERT(distance == 5);
  }

  void test_extract_initials() {
    string source("foo bar");
    // This is how it's used in attribute.cpp:321 // dmd 2012/07/01
    char initials[64];
    char * result = extract_initials(initials, source.c_str());
    string dest(result);
    CPPUNIT_ASSERT(dest == string("fb"));
  }

 /**
  * Compare the extracted strings in data[0] to see whether they
  * started with the same letter and whether one contains the other.
  * i.e.
  * "DAVID WILLIAM" vs "DAVID" = 3 ( max score)
  * "DAVID WILLIAM" vs "WILLIAM" = 0 (min score, not starting with the same letters)
  * "DAVID WILLIAM" vs "DAVE" = 0 ( either one does not container the other. )
  * "" vs "" = 2 ( both missing information )
  * "DAVID" vs "" = 1 ( one missing information )
  */
  void test_midnamecmp() {
    string dw("DAVID WILLIAM");
    string w("WILLIAM");
    string dave("DAVE");
    string david("DAVID");
    string empty("");
    CPPUNIT_ASSERT(3 == midnamecmp(dw, david));
    CPPUNIT_ASSERT(2 == midnamecmp(empty, empty));
    CPPUNIT_ASSERT(1 == midnamecmp(david, empty));
    CPPUNIT_ASSERT(0 == midnamecmp(dw, w));
    CPPUNIT_ASSERT(0 == midnamecmp(dw, dave));
  }


 /**
  * The name_compare function builds a similarity
  * weight.
  */
  void test_name_compare() {

    string s1("foo");
    string s2("bar");
    string s3("baz");
    string s4("foo");
    string s5("");
    string s6("");

    CPPUNIT_ASSERT(4 == name_compare(s1,s4,0,0));
    CPPUNIT_ASSERT(3 == name_compare(s2,s3,0,0));
    CPPUNIT_ASSERT(1 == name_compare(s5,s6,0,0));
    // This is returning zero as part of an abbreviation
    // finding function or something.
    //std::cout << "name_compare: " << name_compare(s1,s3,0,0) << std::endl;
    // TODO: Figure out how to pass through everything such
    // that a result = 2 is returned. 
    //CPPUNIT_ASSERT(2 == name_compare(s1,s3,0,0));
  }

};


void test_comparators() {

  ComparatorsTest * ct = new ComparatorsTest(std::string("initial test"));
  ct->test_zero();
  ct->test_latloncmp();
  ct->test_extract_initials();
  ct->test_midnamecmp();
  ct->test_name_compare();
  delete ct;
}


#ifdef test_comparators_STANDALONE
int
main(int, char **) {
  test_comparators();
  return 0;
}
#endif
