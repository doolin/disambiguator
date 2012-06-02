
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

  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    //delete_attribute();
  }
};

int
main(int argc, char ** argv) {

  ComparatorsTest * ct = new ComparatorsTest(std::string("initial test"));
  ct->runTest();
  ct->test_latloncmp();
  ct->test_zero();
  delete ct;
  return 0;
}
