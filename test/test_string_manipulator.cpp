#include <string>
#include <vector>
#include <iostream>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>

#include "testutils.h"

class StringManipulatorTest : public CppUnit::TestCase {

public:
  StringManipulatorTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  // Postpone this until understanding what it does.
  // Mark it as will not fix in Trac
  void remain_same() {
    StringRemainSame rs();
  }

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
      CPPUNIT_ASSERT(johnson == string("HO"));
    }

    {
      stobj.set_truncater(4, 3, false);
      std::string johnson = stobj.manipulate("JOHNSON");
      CPPUNIT_ASSERT(johnson == string("SNH"));
    }

    {
      stobj.set_truncater(4, 0, false);
      std::string johnson = stobj.manipulate("JOHNSON");
      CPPUNIT_ASSERT(johnson == string(""));
    }

  }

  void collapse_and_truncate() {

    StringNoSpaceTruncate stobj;

    {
      stobj.set_truncater(-6, 2, true);
      std::string johnson = stobj.manipulate("JOHN SON");
      CPPUNIT_ASSERT(johnson == string("OH"));
    }

    {
      stobj.set_truncater(-6, 2, true);
      std::string johnson = stobj.manipulate(" JOHN SON ");
      CPPUNIT_ASSERT(johnson == string("OH"));
    }

  }

  void extract_initials() {

    ExtractInitials eiobj(3);
    std::string result = eiobj.manipulate("THIS IS AN EXAMPLE, YOU KNOW.");
    CPPUNIT_ASSERT(result == string("EYK"));
  }


  void extract_first_word() {

    StringExtractFirstWord sefobj;
    std::string thomas = sefobj.manipulate("THOMAS DAVID ANDERSON");
    CPPUNIT_ASSERT(thomas == std::string("THOMAS"));
  }


  void runTest() {
    remain_same();
    remove_space();
    truncate();
    collapse_and_truncate();
    extract_initials();
    extract_first_word();
  }
};

int
main(int, char **) {

  StringManipulatorTest * rt = new StringManipulatorTest(std::string("String manipulator test"));
  rt->runTest();
  delete rt;
  return 0;
}
