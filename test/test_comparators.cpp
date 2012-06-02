
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <comparators.h>


class ComparatorsTest : public CppUnit::TestCase {

public:
  ComparatorsTest(std::string name) : CppUnit::TestCase(name) {}

  void delete_attribute() {
    //Comparators * c = new Comparators();
    //cFirstname * c = new cFirstname();
    //delete a;
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
