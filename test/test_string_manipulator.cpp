#include <string>
#include <vector>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <DisambigEngine.h>


class StringManipulatorTest : public CppUnit::TestCase { 

public: 
  StringManipulatorTest( std::string name ) : CppUnit::TestCase( name ) {}

  void remove_space() {
    std::string input("THIS IS AN   EXAMPLE  ");
    std::string target("THISISANEXAMPLE");
    StringRemoveSpace * rc = new StringRemoveSpace();
    std::string result = rc->manipulate(input);
    CPPUNIT_ASSERT(result == target);
    delete rc;
  }
 
  void runTest() {
    CPPUNIT_ASSERT( 1  == 1 );
    remove_space();
  }
};

int
main(int argc, char ** argv) {

  StringManipulatorTest * rt = new StringManipulatorTest(std::string("initial test"));
  rt->runTest();
  delete rt;
  return 0;
}
