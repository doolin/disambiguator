
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

  ComparatorsTest * at = new ComparatorsTest(std::string("initial test"));
  at->runTest();
  delete at;
  return 0;
}
