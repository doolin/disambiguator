
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <DisambigEngine.h>
#include <DisambigCluster.h>
//#include <comparators.h>
#include <attribute.h>
#include <DisambigEngine.h>
#include <DisambigFileOper.h>
#include <Disambigmain.h>
#include <DisambigNewCluster.h>
#include <DisambigPostProcess.h>
#include <DisambigRatios.h>
#include <DisambigTraining.h>
#include <DisambigUtilities.h>
#include <strcmp95.h>
#include <Threading.h>
#include <txt2sqlite3.h>


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
    const string uid_identifier = cUnique_Record_ID::static_get_class_name();
    //uid = string("Unique_Record_ID");
    CPPUNIT_ASSERT( uid_identifier == string("Unique_Record_ID"));
  }
};

int
main(int argc, char ** argv) {

  InitialTest * it = new InitialTest(std::string("initial test"));
  it->runTest();
  return 0;
}
