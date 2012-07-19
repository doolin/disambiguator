
#include <string>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>


#include "strcmp95.h"

/**
 * Run this code with /usr/bin/valgrind ./abbreviations --leak-check=full
 */

void
test_is_abbrev() {

  const char * s1 = "abbreviation";
  const char * s2 = "abbr";
  CPPUNIT_ASSERT (3 == is_abbreviation(s1,s2));
}

void
test_is_not_abbrev() {

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

