
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


class ClusterInfoTest : public CppUnit::TestCase {

private:

  list<Record> source;
  vector<string> requested_columns;
  RecordPList record_pointers;
  // Accessory container for unit testing, not
  // present in the disambiguation code.
  vector<Record *> rpv;


public:

  ClusterInfoTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
    requested_columns.push_back(string("Firstname"));
    requested_columns.push_back(string("Lastname"));
    requested_columns.push_back(string("Middlename"));
    requested_columns.push_back(string("Patent"));
    requested_columns.push_back(string("Assignee"));
    requested_columns.push_back(string("AsgNum"));
    requested_columns.push_back(string("ApplyYear"));
    requested_columns.push_back(string("Latitude"));
    requested_columns.push_back(string("Longitude"));
    requested_columns.push_back(string("Street"));
    requested_columns.push_back(string("Country"));
    requested_columns.push_back(string("Unique_Record_ID"));
  }

  void load_fake_data() {

    describe_test(INDENT2, "Loading fake data for clustering...");

    const char * filename = "testdata/clustertest.csv";
    bool successful = fetch_records_from_txt(source, filename, requested_columns);

    if (not successful)
      exit(-1);

    create_record_plist(source, record_pointers);

    // IPDict
    map<string, const Record *> uid_dict;
    const string uid_identifier = cUnique_Record_ID::static_get_class_name();
    create_btree_uid2record_pointer(uid_dict, source, uid_identifier);

    bool matching_mode         = true;
    bool frequency_adjust_mode = false;
    bool debug_mode            = false;
    ClusterInfo match(uid_dict, matching_mode, frequency_adjust_mode, debug_mode);

    const uint32_t num_coauthors_to_group = 2;
    cBlocking_Operation_By_Coauthors blocker_coauthor(record_pointers, num_coauthors_to_group);

    //cBlocking_Operation_By_Coauthors  blocker_coauthor = get_blocker_coathor();
    Cluster::set_reference_patent_tree_pointer(blocker_coauthor.get_patent_tree());

    list<Record>::iterator i = source.begin();
    for (; i != source.end(); ++i) {
      rpv.push_back(&(*i));
    }

    vector<string> comparators;
    comparators.push_back("Firstname");
    comparators.push_back("Lastname");
    Record::activate_comparators_by_name(comparators);
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

    Record * r2 = rpv[2];
    Record * r3 = rpv[3];
    Record * r4 = rpv[4];
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
    load_fake_data();
    //create_cluster();
    test_get_initial_prior();
  }
};


void
test_clusters() {

  ClusterInfoTest * cit = new ClusterInfoTest(std::string("ClusterInfo test"));
  cit->runTest();
  delete cit;
}


#ifdef test_clusterinfo_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusters();
  return 0;
}
#endif
