
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>


#include <attribute.h>
#include <engine.h>
#include <strcmp95.h>
#include <txt2sqlite3.h>


#include <disambiguation.h>

/**
 * Run thie code with /usr/bin/valgrind ./typedefs --leak-check=full
 */

typedef map <string, const Record *> Dict;

// From: https://github.com/doolin/CPP_Disambiguation/blob/master/src/DisambigComp.cpp#L571
//const map<string, std::pair<string, unsigned int> > * const asg_table_pointer
typedef std::pair<std::string, unsigned int> TableVal;
typedef std::map<std::string, TableVal> Table;
const Table * const asg_table_pointer = NULL;

//typedef vector <string> Label;

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
