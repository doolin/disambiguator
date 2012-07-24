
#include <string>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>


#include "strcmp95.h"

/**
 * Run this code with /usr/bin/valgrind ./abbreviation --leak-check=full
 */

void
test_is_abbrev() {

  const char * s1 = "abbreviation";
  const char * s2 = "abbr";
  CPPUNIT_ASSERT (4 == is_abbreviation(s1,s2));
}

/**
 * This tests the implementation, which passes, with
 * the caveat that it doesn't implement an algorithm
 * conforming to any standard, accepted definition of
 * an abbreviation. This might could be called something
 * else, like "custom_stemming" or something.
 */
void
test_is_not_abbrev() {

  const char * s1 = "abbreviation";
  const char * s2 = "abbr.";
  CPPUNIT_ASSERT (0 == is_abbreviation(s1,s2));
}

void
test_abbreviations() {
  test_is_abbrev();
  test_is_not_abbrev();
}

int
main(int argc, char ** argv) {

  test_abbreviations();
  return 0;
}

