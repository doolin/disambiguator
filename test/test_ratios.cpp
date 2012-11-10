#include <string>
#include <iostream>

#include <cppunit/TestCase.h>

#include "ratios.h"

#include "testutils.h"

using std::string;
using std::pair;


class RatioSmoothingTest : public CppUnit::TestCase {

private:

  static const short BUF_SIZE = 256;
  Describer describer;


public:
  RatioSmoothingTest(string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  /*
  Describer it (std::function<bool()> test) {

    Describer d;
    try {
      CPPUNIT_ASSERT(test());
      d = describe_pass;
    } catch (CppUnit::Exception e) {
      d = describe_fail;
    }
    return d;
  }
  */


  void test_compute_total_nodes() {

    Spec spec;

    spec.it("Computing total nodes %d", [&spec](Description desc)->bool {
      SimilarityProfile max;
      SimilarityProfile min;
      uint32_t totalnodes = compute_total_nodes(min, max);
      sprintf(spec.buf, desc, totalnodes);
      return (1 == totalnodes);
    });


    spec.it("Computing total nodes, should equal 4", [](Description d)->bool {
      SimilarityProfile max{1, 1};
      SimilarityProfile min{0, 0};
      return (4 == compute_total_nodes(min, max));
    });


    spec.it("Computing total nodes, should equal 4 (with #define DO)", DO_SPEC {
      SimilarityProfile max{1, 1};
      SimilarityProfile min{0, 0};
      return (4 == compute_total_nodes(min, max));
    });


    spec.it("Max (1,1,2) and min(0,0,0) should have 12 nodes", DO_SPEC {
        SimilarityProfile max{1, 1, 2};
        SimilarityProfile min{0, 0, 0};
        return (12 == compute_total_nodes(min, max));
    });

  }

  void test_get_max_similarity() {

    //SimilarityProfile sp = get_max_similarity(names);
  }

  void test_ratios() {

    test_compute_total_nodes();
    test_get_max_similarity();
  }

};


void test_ratio_smoothing() {

  RatioSmoothingTest * rt = new RatioSmoothingTest(std::string("Ratios test"));
  rt->test_ratios();
  delete rt;
}


#ifdef test_ratios_STANDALONE
int
main(int, char **) {
  test_ratio_smoothing();
  return 0;
}
#endif
