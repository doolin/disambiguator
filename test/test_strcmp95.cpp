
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include "strcmp95.h"

#include "testutils.h"


using std::string;

// Need a tempate for comparing floats
bool
is_equal(float f1, float f2, float tol) {
  return (fabs(f1-f2) < tol);
}

class Strcmp95Test : public CppUnit::TestCase {

private:
  std::stringstream description;

public:
  Strcmp95Test(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  float compute_jw(const char * s1, const char * s2) {
    return strcmp95_modified(s1, s2);
  }

  float compute_jw(string s1, string s2) {
    return strcmp95_modified(s1.c_str(), s2.c_str());
  }

  void print_comparison(string s1, string s2, float value) {
    std::cout << s1 << " vs. " << s2 << ": " << value << std::endl;
  }

  std::string describe_comparison(string s1, string s2, float value) {
    std::stringstream desc;
    desc << s1 << " vs. " << s2 << ": " << value;
    return desc.str();
  }


  void test_identical() {

    string s1("foo"), s2("foo");
    float val = compute_jw(s1, s2);
    //print_comparison(s1, s2, val);
    CPPUNIT_ASSERT (is_equal(val, 1.0, 0.000001));
    describe_pass(INDENT2, "Jaro/Winkler returned 1 for identical strings");
  }


  void test_different() {

    string s1("foo"), s2("bar");
    float val = compute_jw(s1, s2);
    //print_comparison(s1, s2, val);
    CPPUNIT_ASSERT (is_equal(val, 0.0, 0.000001));
    describe_pass(INDENT2, "Jaro/Winkler returned 0 for totally different strings");
  }


  void test_permuted() {

    // The following were extracted from an appendix of an
    // early version of the disambiguation paper.
    string s1("MATTHEW");
    string s2("HEWMATT");

    float val = compute_jw(s1, s2);
    string desc = string("Jaro/Winkler: ") + describe_comparison(s1, s2, val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (val, 0.4857, 0.0001);
    describe_pass(INDENT2, desc.c_str());

    val = compute_jw(s2, s1);
    desc = string("Jaro/Winkler: ") + describe_comparison(s2, s1, val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (val, 0.4857, 0.0001);
    describe_pass(INDENT2, desc.c_str());
  }


  void test_the_rest() {

    string s1("");
    string s2("");

    float val;

    s1 = "MATTHEW";
    s2 = "MATEW";
    val = compute_jw(s1,s2);
    string desc = string("Jaro/Winkler: ") + describe_comparison(s2, s1, val);
    describe_pass(INDENT2, desc.c_str());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (val, 0.94166, 0.0001);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s2 = "MATT";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s2 = "MTATWEH";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s2 = "M";
    val = compute_jw("MATTHEW", "M");
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s2 = "TALIN";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s2 = "XYZ";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    // The next three come from Wikipedia
    s1 = "dixon";
    s2 = "dicksonx";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s1 = "duane";
    s2 = "dwayne";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));

    s1 = "MARTHA";
    s2 = "MARHTA";
    val = compute_jw(s1, s2);
    //st->print_comparison(s1, s2, val);
    //CPPUNIT_ASSERT (is_equal(val, 92.5, 0.000001));
  }

};


void
test_strcmp95() {

  Strcmp95Test * st = new Strcmp95Test(std::string("Jaro/Winkler comparison test"));
  st->test_identical();
  st->test_different();
  st->test_permuted();
  st->test_the_rest();

  delete st;
}


#ifdef test_strcmp95_STANDALONE
int
main(int, char **) {
  test_strcmp95();
  return 0;
}
#endif
