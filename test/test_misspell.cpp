
#include <string>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>


#include "strcmp95.h"

/**
 * Run this code with /usr/bin/valgrind ./missing --leak-check=full
 */

void
test_is_misspell() {

  const char * s1 = "misspell";
  const char * s2 = "mispell";
  CPPUNIT_ASSERT (1 == is_misspell(s1,s2));
}


void
test_is_not_misspell() {

  const char * s1 = "misspell";
  const char * s2 = "misspell";
  CPPUNIT_ASSERT (4 == is_misspell(s1,s2));
}


void
test_is_missepll() {

  const char * s1 = "misspell";
  const char * s2 = "missepll";
  CPPUNIT_ASSERT (2 == is_misspell(s1,s2));
}


void
test_is_misspall() {

  const char * s1 = "misspell";
  const char * s2 = "misspall";
  CPPUNIT_ASSERT (3 == is_misspell(s1,s2));
}


void
test_misspell() {
  test_is_misspell();
  test_is_not_misspell();
  test_is_missepll();
  test_is_misspall();
}

#ifdef misspell_STANDALONE
int
main(int argc, char ** argv) {

  test_misspell();
  return 0;
}
#endif
