#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>
#include <clusterinfo.h>
#include <training.h>
#include <ratios.h>

#include "testdata.h"
#include "testutils.h"


class RatioComponentTest : public CppUnit::TestCase {

//private:

public:

  RatioComponentTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
  }

  void load_fake_data() {

  }


  void runTest() {
    load_fake_data();
  }
};


void
test_ratiocomponent() {

  RatioComponentTest * rt = new RatioComponentTest(std::string("RatioComponent unit tests"));
  rt->runTest();
  delete rt;
}


#ifdef test_ratiocomponent_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_ratiocomponent();
  return 0;
}
#endif
