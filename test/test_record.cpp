
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <disambiguation.h>

#include <DisambigEngine.h>



class RecordTest : public CppUnit::TestCase { 

public: 
  RecordTest( std::string name ) : CppUnit::TestCase( name ) {}

  void delete_record() {
    Record * rc = new Record();
    delete rc;
  }
 
  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_record();
  }
};

int
main(int argc, char ** argv) {

  RecordTest * rt = new RecordTest(std::string("initial test"));
  rt->runTest();
  delete rt;
  return 0;
}
