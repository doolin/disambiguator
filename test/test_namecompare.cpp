
#include <string>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>


#include "comparators.h"

/**
 * Run this code with /usr/bin/valgrind ./namecompare --leak-check=full
 */

void
test_empty() {

  std::string s1("");
  std::string s2("");
  CPPUNIT_ASSERT (1 == name_compare(s1,s2,0,0));
}


void
test_is_misspell() {

  std::string s1("misspell");
  std::string s2("misspel");
  CPPUNIT_ASSERT (3 == name_compare(s1,s2,0,0));
}


void
test_is_not_misspell() {

  const char * s1 = "misspell";
  const char * s2 = "misspell";
  CPPUNIT_ASSERT (4 == name_compare(s1,s2,0,0));
}


void
test_is_missepll() {

  std::string s1("misspell");
  std::string s2("missepll");
  CPPUNIT_ASSERT (3 == name_compare(s1,s2,0,0));
}


void
test_is_misspall() {

  std::string s1("misspell");
  std::string s2("misspall");
  CPPUNIT_ASSERT (3 == name_compare(s1,s2,0,0));
}


void
test_namecompare() {
  test_empty();
  test_is_misspell();
  test_is_not_misspell();
  test_is_missepll();
  test_is_misspall();
}

#ifdef namecompare_STANDALONE
int
main(int argc, char ** argv) {

  test_namecompare();
  return 0;
}
#endif
