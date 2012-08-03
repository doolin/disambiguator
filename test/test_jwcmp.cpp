
#include <string>
#include <iostream>
#include <cmath>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include "comparators.h"

using std::string;

// Need a tempate for comparing floats
bool
is_equal(float f1, float f2, float tol) {
  return (fabs(f1-f2) < tol);
}

class JWcmpTest : public CppUnit::TestCase {

public:
  JWcmpTest(std::string name) : CppUnit::TestCase(name) {}

  float compute_jw(const char * s1, const char * s2) {
    return jwcmp(s1, s2);
  }

  float compute_jw(string s1, string s2) {
    return jwcmp(s1.c_str(), s2.c_str());
  }

  void print_comparison(string s1, string s2, float value) {
    std::cout << s1 << " vs. " << s2 << ": " << value << std::endl;
  }

};


void
test_jwcmp() {

  JWcmpTest * st = new JWcmpTest(std::string("initial test"));

  string s1("");
  string s2("");

  delete st;
}


#ifdef jwcmp_STANDALONE
int
main(int argc, char ** argv) {
  test_jwcmp();
  return 0;
}
#endif /* jwcmp_STANDALONE */

