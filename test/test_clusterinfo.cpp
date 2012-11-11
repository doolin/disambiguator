
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

  list<Record> source;
  vector<string> requested_columns;
  RecordPList recpointers;
  // Accessory container for unit testing, not
  // present in the disambiguation code.
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


  void create_cluster() {

    describe_test(INDENT2, "Creating clusters...");

    Attribute::register_class_names(get_column_names());
    Record * r = make_quuxalot_record();

    //ClusterHead ch(r, 0.9953);
    //RecordPList rl = get_record_list();
    cBlocking_Operation_By_Coauthors  blocker_coauthor = get_blocker_coathor();
    Cluster::set_reference_patent_tree_pointer(blocker_coauthor.get_patent_tree());

    //Cluster * c = new Cluster(ch, rl);
    //Cluster * c = new Cluster(ch, record_pointers);
    //delete c;
  }

  void test_get_initial_prior() {

    describe_test(INDENT2, "Testing get_initial_prior");

    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];
    const Record * r4 = rpv[4];
    //r4->print();

    ClusterHead ch(r2, 0.9953);
    RecordPList rpl1;
    rpl1.push_back(r2);

    RecordPList rpl2;
    rpl2.push_back(r3);
    rpl2.push_back(r4);

    Cluster c1 = Cluster(ch, rpl1);
    Cluster c2 = Cluster(ch, rpl2);

    ClusterInfo::ClusterList rg;

    rg.push_back(c1);
    rg.push_back(c2);

    double prior = get_initial_prior(rg, false);
    //std::cout << "prior: " << prior << std::endl;

    CPPUNIT_ASSERT ((0.333333-prior) < 0.00001);

  }

  void runTest() {
    //create_cluster();
    test_get_initial_prior();
  }
};


void
test_clusterinfo() {

  ClusterInfoTest * cit = new ClusterInfoTest(std::string("ClusterInfo test"));
  cit->runTest();
  delete cit;
}


#ifdef test_clusterinfo_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusterinfo();
  return 0;
}
#endif
