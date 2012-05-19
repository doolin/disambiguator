#include <string>
#include <vector>
#include <iostream>

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


  // The code calls this operation "Truncate" but
  // it's really a substring operation. I thought
  // these kinds of operations were in the STL.
  void truncate() {

    StringTruncate stobj;

    {
      stobj.set_truncater(0, 5, true);
      std::string eric = stobj.manipulate ("ERIC");
      CPPUNIT_ASSERT(eric == string("ERIC"));
      std::string johnson = stobj.manipulate ("JOHNSON");
      //std::cout << johnson << std::endl;
      CPPUNIT_ASSERT(johnson == string("JOHNS"));
    }

    {
      // starting position = 0 (head of the string),
      // extraction length = full length, direction = forward.
      stobj.set_truncater(0, 0, true);
      std::string johnson =  stobj.manipulate ("JOHNSON");
      CPPUNIT_ASSERT(johnson == string("JOHNSON"));
    }

    {
      stobj.set_truncater(-6, 2, true);
      std::string johnson = stobj.manipulate("JOHNSON");
      CPPUNIT_ASSERT(johnson == string("OH"));
    }

    {
      stobj.set_truncater(-5, 2, false);
      std::string johnson = stobj.manipulate("JOHNSON");
      //std::cout << johnson << std::endl;
      CPPUNIT_ASSERT(johnson == string("HO"));
    }

    {
      stobj.set_truncater(4, 3, false);
      std::string johnson = stobj.manipulate("JOHNSON");
      //std::cout << johnson << std::endl;
      CPPUNIT_ASSERT(johnson == string("SNH"));
    }

    {
      stobj.set_truncater(4, 0, false);
      std::string johnson = stobj.manipulate("JOHNSON");
      //std::cout << johnson << std::endl;
      CPPUNIT_ASSERT(johnson == string(""));
    }

  }
 
  void runTest() {
    CPPUNIT_ASSERT( 1  == 1 );
    remove_space();
    truncate();
  }
};

int
main(int argc, char ** argv) {

  StringManipulatorTest * rt = new StringManipulatorTest(std::string("initial test"));
  rt->runTest();
  delete rt;
  return 0;
}
