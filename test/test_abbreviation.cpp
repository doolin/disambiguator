
#include <string>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>


#include <strcmp95.h>

/**
 * Run this code with /usr/bin/valgrind ./abbreviations --leak-check=full
 */


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
