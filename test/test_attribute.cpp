
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>

// Really good web pages:
// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
// http://stackoverflow.com/questions/7182359/template-instantiation-details-of-gcc-and-ms-compilers
#include <disambiguation.h>
#include <engine.h>
#include <attribute.h>


class AttributeTest : public CppUnit::TestCase { 

public: 
  AttributeTest( std::string name ) : CppUnit::TestCase( name ) {}

  void delete_attribute() {
    //Attribute * a = new Attribute();
    cFirstname * a = new cFirstname();
    delete a;
  }
 
  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_attribute();
  }
};

int
main(int UP(argc), char ** UP(argv)) {

  AttributeTest * at = new AttributeTest(std::string("initial test"));
  at->runTest();
  delete at;
  return 0;
}
