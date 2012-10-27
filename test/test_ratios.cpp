#include <string>
#include <iostream>

#include <cppunit/TestCase.h>

#include "ratios.h"

//extern "C" {
//  #include "strcmp95.h"
//}

using std::string;
using std::pair;


class RatioSmoothingTest : public CppUnit::TestCase {

public:
  RatioSmoothingTest(string name) : CppUnit::TestCase(name) {}

  void test_compute_total_nodes() {
    SimilarityProfile max;
    SimilarityProfile min;
    uint32_t tn = compute_total_nodes(min, max);
    CPPUNIT_ASSERT(1 == tn);
    max.push_back(1);
    max.push_back(1);
    min.push_back(0);
    min.push_back(0);
    tn = compute_total_nodes(min, max);
    //std::cout << "tn: " << tn << std::endl;
    CPPUNIT_ASSERT(4 == tn);
    max.push_back(2);
    min.push_back(0);
    tn = compute_total_nodes(min, max);
    //std::cout << "tn: " << tn << std::endl;
    CPPUNIT_ASSERT(12 == tn);
  }

  void test_get_max_similarity() {

    //SimilarityProfile sp = get_max_similarity(names);
  }
};


void test_ratio_smoothing() {

  RatioSmoothingTest * rt = new RatioSmoothingTest(std::string("initial test"));
  rt->test_compute_total_nodes();
  rt->test_get_max_similarity();
  delete rt;
}


#ifdef test_ratios_STANDALONE
int
main(int, char **) {
  test_ratio_smoothing();
  return 0;
}
#endif
