#include <string>
#include <vector>
#include <iostream>

#include <cppunit/TestCase.h>

//#include <disambiguation.h>
//#include <engine.h>


#include <typedefs.h>
#include <ratios.h>

#include "testutils.h"

typedef std::pair<SimilarityProfile, double> Ratio;
typedef std::pair<SimilarityProfile, uint32_t> Count;
typedef SimilarityProfile SP;

SimilarityProfile
csp(uint32_t s1, uint32_t s2) {
  SimilarityProfile sp;
  sp.push_back(s1);
  sp.push_back(s2);
  return sp;
}


class QPTest : public CppUnit::TestCase {

private:
    SimilarityProfile sp, max, min;
    SPRatiosIndex ratio_map;
    SPCountsIndex mcount, ncount;
    SP sp1, sp2, sp3, sp4, sp5, sp6, sp7, sp8;

    void init_mcounts() {

      SP sp1 = csp(1,0);
      SP sp2 = csp(0,1);
      SP sp3 = csp(2,0);
      SP sp4 = csp(2,2);
      SP sp5 = csp(2,3);
      SP sp6 = csp(1,2);
      SP sp7 = csp(1,3);
      SP sp8 = csp(3,1);

      mcount.insert(Count(sp1, 5));
      mcount.insert(Count(sp2, 2));
      mcount.insert(Count(sp3, 20));
      mcount.insert(Count(sp4, 35));
      mcount.insert(Count(sp5, 101));
      mcount.insert(Count(sp6, 8));
      mcount.insert(Count(sp7, 7));
      mcount.insert(Count(sp8, 201));

    }


    void init_ncounts() {

      SP sp1 = csp(1,0);
      SP sp2 = csp(0,1);
      SP sp3 = csp(2,0);
      SP sp4 = csp(2,2);
      SP sp5 = csp(2,3);
      SP sp6 = csp(1,2);
      SP sp7 = csp(1,3);
      SP sp8 = csp(3,1);

      ncount.insert(Count(sp1, 5.0));
      ncount.insert(Count(sp2, 2.0));
      ncount.insert(Count(sp3, 20.0));
      ncount.insert(Count(sp4, 35.0));
      ncount.insert(Count(sp5, 101.0));
      ncount.insert(Count(sp6, 8.0));
      ncount.insert(Count(sp7, 7.0));
      ncount.insert(Count(sp8, 201.0));

    }

public:
  QPTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());

    max = csp(3,3);
    min = csp(0,0);

    sp1 = csp(1,0);
    sp2 = csp(0,1);
    sp3 = csp(2,0);
    sp4 = csp(2,2);
    sp5 = csp(2,3);
    sp6 = csp(1,2);
    sp7 = csp(1,3);
    sp8 = csp(3,1);

    ratio_map.insert(Ratio(sp1, 5.0));
    ratio_map.insert(Ratio(sp2, 2.0));
    ratio_map.insert(Ratio(sp3, 20.0));
    ratio_map.insert(Ratio(sp4, 35.0));
    ratio_map.insert(Ratio(sp5, 101.0));
    ratio_map.insert(Ratio(sp6, 8.0));
    ratio_map.insert(Ratio(sp7, 7.0));
    ratio_map.insert(Ratio(sp8, 201.0));

    init_mcounts();
    init_ncounts();
  }

  void print_indexes() {

    SPRatiosIndex::const_iterator it = ratio_map.begin();
    for (; it != ratio_map.end(); ++it) {
      uint32_t index = sp2index(it->first, min, max);
      std::cout << "index: " << index << ", ";
      print_similarity(it->first);
      std::cout << ", " << it->second;
      std::cout << std::endl;
    }

  }

  void test_smoothing() {

    vector<string> attribute_names;
    attribute_names.push_back(string("foo"));

    smoothing_inter_extrapolation_cplex(ratio_map, min, max, mcount, ncount, attribute_names, false, false);
  }


  void runTest() {
    print_indexes();
    test_smoothing();
    print_indexes();
  }

};


void
test_qp() {

  QPTest * qpt = new QPTest(std::string("Similarity test"));
  qpt->runTest();
  delete qpt;
}


#ifdef test_qp_STANDALONE
int
main(int, char **) {

  test_qp();
  return 0;
}
#endif
