
#include <string>
#include <iostream>
#include <cmath>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include "strcmp95.h"

using std::string;

// Need a tempate for comparing floats
bool
is_equal(float f1, float f2, float tol) {
  return (fabs(f1-f2) < tol);
}

class Strcmp95Test : public CppUnit::TestCase {

public:
  Strcmp95Test(std::string name) : CppUnit::TestCase(name) {}

  float compute_jw(const char * s1, const char * s2) {
    return strcmp95_modified(s1, s2);
  }

  float compute_jw(string s1, string s2) {
    return strcmp95_modified(s1.c_str(), s2.c_str());
  }

  void print_comparison(string s1, string s2, float value) {
    //std::cout << s1 << " vs. " << s2 << ": " << value << std::endl;
  }

};


void
test_strcmp95() {

  Strcmp95Test * st = new Strcmp95Test(std::string("initial test"));

  string s1("");
  string s2("");

  float val = st->compute_jw("foo", "foo");
  //std::cout << "val: " << val << std::endl;
  CPPUNIT_ASSERT (is_equal(val, 1.0, 0.000001));

  val = st->compute_jw("bar", "foo");
  //std::cout << "val: " << val << std::endl;
  CPPUNIT_ASSERT (is_equal(val, 0.0, 0.000001));

  // The following were extracted from an appendix of an
  // early version of the disambiguation paper.
  s1 = "MATTHEW";
  s2 = "HEWMATT";
  val = st->compute_jw("MATTHEW", "HEWMATT");
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "MATEW";
  val = st->compute_jw(s1,s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "MATT";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "MTATWEH";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "M";
  val = st->compute_jw("MATTHEW", "M");
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "TALIN";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s2 = "XYZ";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  // The next three come from Wikipedia
  s1 = "dixon";
  s2 = "dicksonx";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s1 = "duane";
  s2 = "dwayne";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  s1 = "MARTHA";
  s2 = "MARHTA";
  val = st->compute_jw(s1, s2);
  st->print_comparison(s1, s2, val);
  //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

  delete st;
}


int
main(int argc, char ** argv) {
  test_strcmp95();
  return 0;
}
