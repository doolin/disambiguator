
#include <string>

#include <cppunit/TestCase.h>

#include "comparators.h"
#include "testutils.h"


/**
 * Run this code with /usr/bin/valgrind ./missing --leak-check=full
 */

class Misspell : public CppUnit::TestCase {

public:
  Misspell(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }


    void test_is_misspell() {

      Spec spec;
      spec.it("Testing 'misspell' against 'mispell' returns 1", DO_SPEC {
        const char * s1 = "misspell";
        const char * s2 = "mispell";
        return (1 == is_misspell(s1,s2));
      });
    }


    void test_is_not_misspell() {

      Spec spec;
      spec.it("Testing 'misspell' against 'misspell' returns 4", DO_SPEC {
        const char * s1 = "misspell";
        const char * s2 = "misspell";
        return (4 == is_misspell(s1,s2));
      });
    }


    void test_is_missepll() {

      Spec spec;
      spec.it("Testing 'misspell' against missepll' returns 2", DO_SPEC {
        const char * s1 = "misspell";
        const char * s2 = "missepll";
        return (2 == is_misspell(s1,s2));
      });
    }


    void test_is_misspall() {

      Spec spec;
      spec.it("Testing 'misspell' against 'misspall' returns 3", DO_SPEC {
        const char * s1 = "misspell";
        const char * s2 = "misspall";
        return (3 == is_misspell(s1,s2));
      });
    }



  void runTests() {
    test_is_misspell();
    test_is_not_misspell();
    test_is_missepll();
    test_is_misspall();
  }

};



void
test_misspell() {
  Misspell * mt = new Misspell("Misspell (is_misspell()) function unit test");
  mt->runTests();
  delete mt;
}

#ifdef test_misspell_STANDALONE
int
main(int, char **) {

  test_misspell();
  return 0;
}
#endif
