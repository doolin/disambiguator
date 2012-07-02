
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>
using std::string;

#include <comparators.h>


class ComparatorsTest : public CppUnit::TestCase {

public:
  ComparatorsTest(std::string name) : CppUnit::TestCase(name) {}

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

};


void test_comparators() {

  ComparatorsTest * ct = new ComparatorsTest(std::string("initial test"));
  ct->test_zero();
  ct->test_latloncmp();
  ct->test_extract_initials();
  delete ct;
}



int
main(int argc, char ** argv) {
  test_comparators();
  return 0;
}
