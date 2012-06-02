
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


class Initial {
public:
  Initial(void) : value(1) {}
  int value;
};

class InitialTest : public CppUnit::TestCase {
public:
  InitialTest( std::string name ) : CppUnit::TestCase( name ) {}

  void runTest() {
    Initial * i = new Initial();
    CPPUNIT_ASSERT( i->value  == 1 );
  }
};

int
main(int argc, char ** argv) {

  InitialTest * it = new InitialTest(std::string("initial test"));
  it->runTest();
  return 0;
}
