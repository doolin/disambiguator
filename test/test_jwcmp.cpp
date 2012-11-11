
#include <string>
#include <iostream>
#include <cmath>

#include <cppunit/TestCase.h>

#include "comparators.h"

#include "testutils.h"

using std::string;


class JWcmpTest : public CppUnit::TestCase {

private:
  string s1, s2;

public:
  JWcmpTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  float compute_jw(const char * s1, const char * s2) {
    return jwcmp(s1, s2);
  }

  float compute_jw(string s1, string s2) {
    return jwcmp(s1.c_str(), s2.c_str());
  }

  void print_score(string s1, string s2, int score) {
    std::cout << s1 << " vs. " << s2 << ": " << score << std::endl;
  }


  void testem_all() {

    Spec spec;

    spec.it("Comparing two empty strings scores 0", [this](Description desc)->bool {
      s1 = ""; s2 = "";
      int score = jwcmp(s1, s2);
      //print_score(s1, s2, score);
      return (0 == score);
    });


    spec.it("Comparing %s with %s results in %d", [this, &spec](Description desc)->bool {
      s1 = "MATTHEW"; s2 = "XYZ";
      int score = jwcmp(s1, s2);
      sprintf(spec.buf, desc, s1.c_str(), s2.c_str(), score);
      return (0 == score);
    });

    spec.it("Comparing %s with %s results in %d", [this, &spec](Description desc)->bool {
      s1 = "MATTHEW"; s2 = "TALIN";
      int score = jwcmp(s1, s2);
      sprintf(spec.buf, desc, s1.c_str(), s2.c_str(), score);
      return (0 == score);
    });


    spec.it("Comparing %s with %s results in %d", [this, &spec](Description desc)->bool {
      s1 = "MATTHEW"; s2 = "MATHEW";
      int score = jwcmp(s1, s2);
      sprintf(spec.buf, desc, s1.c_str(), s2.c_str(), score);
      return (4 == score);
    });


    spec.it("Comparing %s with %s results in %d", [this, &spec](Description desc)->bool {
      s1 = "MATTHEW"; s2 = "MATTHEW";
      int score = jwcmp(s1, s2);
      sprintf(spec.buf, desc, s1.c_str(), s2.c_str(), score);
      return (5 == score);
    });

  }

  void runTests() {
    testem_all();
  }

};


void
test_jwcmp() {

  JWcmpTest * st = new JWcmpTest(std::string("Jaro/Winkler comparison unit testing"));
  st->testem_all();

  delete st;
}


#ifdef test_jwcmp_STANDALONE
int
main(int, char **) {
  test_jwcmp();
  return 0;
}
#endif /* jwcmp_STANDALONE */
