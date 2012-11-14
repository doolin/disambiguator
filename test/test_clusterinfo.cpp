
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
  list<Record> all_records;


public:

  ClusterInfoTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
    const string filename("testdata/assignee_comparison.csv");
    ft = new FakeTest(string("Fake ClusterInfo test"), filename);
    ft->load_fake_data(filename);
    recpointers = ft->get_recpointers();
    all_records = ft->get_all_records();
    rpv = ft->get_recvecs();
  }


  void test_get_initial_prior() {

    describe_test(INDENT2, "Testing get_initial_prior");

    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];
    const Record * r4 = rpv[4];
    // Dummy value for cohesion
    double cohesion = 0.2343;
    ClusterHead ch(r2, cohesion);

    RecordPList rpl1 = { r2 };
    Cluster c1 = Cluster(ch, rpl1);

    RecordPList rpl2 = { r3, r4 };
    Cluster c2 = Cluster(ch, rpl2);

    ClusterInfo::ClusterList rg = { c1, c2 };

    double prior = get_initial_prior(rg);

    Spec spec;
    spec.it("get_initial_prior() returns ~0.333333", [&](Description desc)->bool {
      return (fabs(0.333333-prior) < 0.00001);
    });

    rg.clear();
    rg.push_back(c2);
    rg.push_back(c1);

    spec.it("Reorder and get_initial_prior() returns ~0.333333", [&](Description desc)->bool {
      return (fabs(0.333333-prior) < 0.00001);
    });

  }


  void test_get_initial_prior2() {

    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];
    const Record * r4 = rpv[4];
    const Record * r5 = rpv[5];
    const Record * r6 = rpv[6];
    const Record * r7 = rpv[7];
    const Record * r8 = rpv[8];
    // Dummy value for cohesion
    double cohesion = 0.2343;
    ClusterHead ch(r2, cohesion);

    RecordPList rpl1 = { r2, r3, r8 };
    Cluster c1 = Cluster(ch, rpl1);
    RecordPList rpl2 = { r4, r5, r6, r7 };
    Cluster c2 = Cluster(ch, rpl2);
    ClusterInfo::ClusterList rg = { c1, c2 };

    double prior = get_initial_prior(rg);

    Spec spec;
    spec.it("get_initial_prior() returns ~0.428571", [&](Description desc)->bool {
      //return ((0.333333-prior) < 0.00001);
      return (fabs(0.428571-prior) < 0.00001);
    });
  }

  void test_constructor() {

    describe_test(INDENT2, "Test the ClusterInfo constructor");

    map<string, const Record *>  uid_dict;
    const string uid_identifier = cUnique_Record_ID::static_get_class_name();
    create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);
    ClusterInfo match(uid_dict, true, true, false);
    Spec spec;
    spec.xit("ClusterInfo constructor works", DO_SPEC {
        return true;
    });
  }


  void test_adjust_prior() {

    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];
    const Record * r4 = rpv[4];
    const Record * r5 = rpv[5];
    const Record * r6 = rpv[6];
    const Record * r7 = rpv[7];
    const Record * r8 = rpv[8];
    // Dummy value for cohesion
    double cohesion = 0.2343;
    ClusterHead ch(r2, cohesion);

    RecordPList rpl1 = { r2, r3, r8 };
    Cluster c1 = Cluster(ch, rpl1);
    RecordPList rpl2 = { r4, r5, r6, r7 };
    Cluster c2 = Cluster(ch, rpl2);
    ClusterInfo::ClusterList rg = { c1, c2 };

    map<string, const Record *>  uid_dict;
    const string uid_identifier = cUnique_Record_ID::static_get_class_name();
    create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);
    ClusterInfo match(uid_dict, true, true, false);

    double adjusted = match.adjust_prior(rg, string("FOOBAR###BAZ"), 0.333, false);

    Spec spec;
    spec.xit("adjust_prior() should do nothing", DO_SPEC {
        return true;
    });
  }

  void runTests() {
    test_get_initial_prior();
    test_get_initial_prior2();
    test_adjust_prior();
    test_constructor();
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
