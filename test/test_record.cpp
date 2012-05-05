
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <DisambigEngine.h>
#include <DisambigCluster.h>
#include <DisambigComp.h>
#include <DisambigCustomizedDefs.h>
#include <DisambigDefs.h>
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



class RecordTest : public CppUnit::TestCase { 

public: 
  RecordTest( std::string name ) : CppUnit::TestCase( name ) {}

  void delete_record() {
    cRecord * rc = new cRecord();
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
