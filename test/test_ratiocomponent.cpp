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
#include "fake.h"

class RatioComponentTest : public CppUnit::TestCase {

private:
    cRatioComponent * rc;
    FakeTest * ft;

public:

  RatioComponentTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
    const string filename("testdata/clustertest.csv");
    ft = new FakeTest(string("Fake RatioComponentTest"), filename);
    RecordIndex * uid_dict = ft->get_uid_dict();
    rc = new cRatioComponent(*uid_dict, string("Patent"));
  }

 ~RatioComponentTest() {
    delete ft;
  }

  void load_fake_data() {
    const string filename("testdata/clustertest.csv");
    ft->load_fake_data(filename);
  }

  void test_create_ratios() {

    // Create x_counts and m_counts
    // maybe clear ratio_map.
    // call rc->create_ratios();
    // Find a way to test the result of that call.
  }

  void runTest() {
    load_fake_data();
    test_create_ratios();
  }
};


void
test_ratiocomponent() {

  string title = "Fake object for RatioComponent";
  string csvfilename = "";
  RatioComponentTest * rt = new RatioComponentTest(string("RatioComponent test"));
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
