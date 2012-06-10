
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>
#include <vector>

#include <DisambigEngine.h>
#include <attribute.h>
#include <DisambigEngine.h>
#include <Disambigmain.h>
#include <DisambigPostProcess.h>
//#include <DisambigRatios.h>
#include <strcmp95.h>
#include <Threading.h>
#include <txt2sqlite3.h>


#if  0
const list<const Record *>  psource(new Record()); 
const vector <string> blocking_column_names(1, "Testem");
const vector<const cString_Manipulator*> pmanipulators(new cString_Manipulator()) 
//onst string & unique_identifier = 

cBlocking::cBlocking (const list<const Record *> & psource,
						const vector<string> & blocking_column_names,
						const vector<const cString_Manipulator*>& pmanipulators,
						const string & unique_identifier);

class BlockingTest : public CppUnit::TestCase { 

public: 
  BlockingTest( std::string name ) : CppUnit::TestCase( name ) {}

  void delete_blocking() {
    cBlocking * rc = new cBlocking();
    delete rc;
  }
 
  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_blocking();
  }
};
#endif

int
main(int argc, char ** argv) {

  //BlockingTest * bt = new BlockingTest(std::string("initial test"));
  //bt->runTest();
  //delete bt;
  return 0;
}

