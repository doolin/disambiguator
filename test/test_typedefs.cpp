
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

/**
 * Run thie code with /usr/bin/valgrind ./typedefs --leak-check=full
 */

typedef map <string, const cRecord *> Dict;

typedef vector <string> Label;

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
    delete i;
  }

  void LabelTest(void) {
    Label * l = new Label();
    delete l;
  }

  void LabelTest(char const * label) {
    const string s(label);
    Label l(1, s);
    Label m(1, string("baz"));
  }
};


int
main(int argc, char ** argv) {

  InitialTest * it = new InitialTest(std::string("initial test"));
  it->runTest();
  it->LabelTest();
  it->LabelTest("foobar");

  delete it;

  return 0;
}
