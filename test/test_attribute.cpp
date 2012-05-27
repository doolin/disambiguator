
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <disambiguation.h>
#include <DisambigEngine.h>
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
main(int argc, char ** argv) {

  AttributeTest * at = new AttributeTest(std::string("initial test"));
  at->runTest();
  delete at;
  return 0;
}
