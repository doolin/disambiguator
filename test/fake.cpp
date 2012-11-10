
#include <string>
#include <vector>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>
#include <clusterinfo.h>
#include <training.h>
#include <record.h>
#include <ratios.h>

#include "fake.h"
#include "testdata.h"
#include "testutils.h"


FakeTest::FakeTest(string name, string filename)
  : CppUnit::TestCase(name), csvfilename(filename) {

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
    requested_columns.push_back(string("Coauthor"));
    requested_columns.push_back(string("Class"));
}


void
FakeTest::load_fake_data(string csvfilename) {

    describe_test(INDENT2, "Loading fake data...");

    //const char * filename = "testdata/clustertest.csv";
    const char * filename = csvfilename.c_str();
    bool successful = fetch_records_from_txt(source, filename, requested_columns);

    if (not successful)
      exit(-1);

    Record * r = &source.front();
    Record::set_sample_record(r);

    create_record_plist(source, record_pointers);

    // IPDict
    //map<string, const Record *> uid_dict;
    const string uid_identifier = cUnique_Record_ID::static_get_class_name();
    create_btree_uid2record_pointer(uid_dict, source, uid_identifier);

    bool matching_mode         = true;
    bool frequency_adjust_mode = false;
    bool debug_mode            = false;
    ClusterInfo match(uid_dict, matching_mode, frequency_adjust_mode, debug_mode);

    const uint32_t num_coauthors_to_group = 2;
    cBlocking_Operation_By_Coauthors blocker_coauthor(record_pointers, num_coauthors_to_group);
    coauthor_blocking = &blocker_coauthor;

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


RecordIndex *
FakeTest::get_uid_dict() {
  return &uid_dict;
}


const cBlocking_Operation_By_Coauthors *
FakeTest::get_coauthor_blocking() {
  return coauthor_blocking;
}


RecordPList
FakeTest::get_recpointers() {
  return record_pointers;
}


vector<const Record *>
FakeTest::get_recvecs() {
  return rpv;
}

void
test_fake() {

  const string filename("testdata/clustertest.csv");
  FakeTest * ft = new FakeTest(std::string("initial test"), filename);
  ft->runTest();
  delete ft;
}


#if 0 // not working anymore, need to build libtest.a
#ifdef fake_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_fake();
  return 0;
}
#endif
#endif
