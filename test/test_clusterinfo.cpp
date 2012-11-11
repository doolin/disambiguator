
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



class ClusterInfoTest : public CppUnit::TestCase {

private:

  FakeTest * ft;
  RecordPList recpointers;
  vector<const Record *> rpv;


public:

  ClusterInfoTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
    const string filename("testdata/assignee_comparison.csv");
    ft = new FakeTest(string("Fake ClusterInfo test"), filename);
    ft->load_fake_data(filename);
    recpointers = ft->get_recpointers();
    rpv = ft->get_recvecs();
  }


  void test_get_initial_prior() {

    describe_test(INDENT2, "Testing get_initial_prior");

    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];
    const Record * r4 = rpv[4];
    double chinit = 0.2343;
    //ClusterHead ch(r2, 0.9953);
    ClusterHead ch(r2, chinit);

    RecordPList rpl1 = { r2 };
    Cluster c1 = Cluster(ch, rpl1);

    RecordPList rpl2 = { r3, r4 };
    Cluster c2 = Cluster(ch, rpl2);

    ClusterInfo::ClusterList rg = { c1, c2 };

    double prior = get_initial_prior(rg, false);

    Spec spec;
    spec.it("The prior should be ~0.333333", [&](Description desc)->bool {
      return ((0.333333-prior) < 0.00001);
    });

  }


  void runTests() {
    test_get_initial_prior();
  }

};


void
test_clusterinfo() {

  ClusterInfoTest * cit = new ClusterInfoTest(std::string("ClusterInfo test"));
  cit->runTests();
  delete cit;
}


#ifdef test_clusterinfo_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusterinfo();
  return 0;
}
#endif
