#include <string>
#include <iostream>

#include <cppunit/TestCase.h>

#include "ratios.h"
#include "engine.h"
#include "block.h"

#include "testutils.h"
#include "fake.h"

using std::string;
using std::pair;


class RatioSmoothingTest : public CppUnit::TestCase {

private:

  static const short BUF_SIZE = 256;
  Describer describer;


public:
  RatioSmoothingTest(string name) : CppUnit::TestCase(name) {

    const string filename("testdata/clustertest.csv");
    FakeTest * ft = new FakeTest(string("Fake RatioComponentTest"), filename);
    ft->load_fake_data(filename);

    describe_test(INDENT0, name.c_str());
  }


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

    Spec spec;

    spec.it("Max similarity should be (4,3,5,6,4,6)", DO_SPEC {
      vector<string> names{"Firstname", "Middlename", "Lastname", "Coauthor", "Class", "Assignee"};
      Record::activate_comparators_by_name(names);
      SimilarityProfile sp = get_max_similarity(names);
      SimilarityProfile max = {4,3,5,6,4,6};
      return (max == get_max_similarity(names));
    });

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
