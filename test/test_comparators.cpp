
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
    // This is how it's done in attribute.cpp:321 // dmd 2012/07/01
    char initials[64];
    char * result = extract_initials(initials, source.c_str());
    string dest(result);
    CPPUNIT_ASSERT(dest == string("fb"));
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
