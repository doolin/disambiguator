
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


/**
 * The case where 2 is returned.
 * Why?
 */
void
test_is_abbrev2() {

  std::string s1("miss");
  std::string s2("misspell");
  CPPUNIT_ASSERT (2 == name_compare(s1,s2,3,0));
}


void
test_namecompare() {
  test_empty();
  test_is_misspell();
  test_is_not_misspell();
  test_is_missepll();
  test_is_misspall();
  test_is_abbrev2();
}


void
print_namecompare(std::string s1, std::string s2, int prev, int cur, int value) {

  std::cout << "s1: "    << s1   << ", "
            << "s2: "    << s2   << ", "
            << "prev: "  << prev << ", "
            << "cur: "   << cur  << ", "
            << "value: " << value
            << std::endl;
}

void
traverse_cursors(std::string s1, std::string s2) {

  for (unsigned int prev=0; prev<s1.size(); ++prev) {
    for (unsigned int cur=0; cur<s2.size(); ++cur) {
      int value = name_compare(s1,s2,prev,cur);
      print_namecompare(s1, s2, prev, cur, value);
    }
  }
}

void
test_positioners() {

  traverse_cursors("fo", "fi");
  traverse_cursors("foo", "foi");
  traverse_cursors("foo", "boo");
  traverse_cursors("foo", "foo");
  traverse_cursors("foobar", "foo");
  traverse_cursors("foo", "foobar");
  traverse_cursors("kim", "lim");
  traverse_cursors("Kim", "lim");
  traverse_cursors("Kim", "Lim");
}


#ifdef namecompare_STANDALONE
int
main(int argc, char ** argv) {

// http://stackoverflow.com/questions/138383/colored-grep/138528#138528
// http://stackoverflow.com/questions/9158150/colored-output-in-c
  std::cout << "\033[32mTesting \033[36mname_\033[33mcompare...!\033[0m" << std::endl;
  test_namecompare();
  test_positioners();
  return 0;
}
#endif
