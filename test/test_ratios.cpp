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

  void test_compute_total_nodes() {

    char buffer[BUF_SIZE];
    char teststr[] = "Computing totalnodes %d";

    SimilarityProfile max;
    SimilarityProfile min;
    uint32_t totalnodes = compute_total_nodes(min, max);
    sprintf(buffer, teststr, totalnodes);

    describer = it([totalnodes] () -> bool {
        return (1 == totalnodes);
    });
    describer(INDENT4, buffer);

    max.push_back(1);
    max.push_back(1);
    min.push_back(0);
    min.push_back(0);
    totalnodes = compute_total_nodes(min, max);
    sprintf(buffer, teststr, totalnodes);

    describer = it([totalnodes] () -> bool {
        return (4 == totalnodes);
    });
    describer(INDENT4, buffer);


    max.push_back(2);
    min.push_back(0);
    totalnodes = compute_total_nodes(min, max);
    sprintf(buffer, teststr, totalnodes);

    describer = it([totalnodes] () -> bool {
        return (12 == totalnodes);
    });
    describer(INDENT4, buffer);

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
