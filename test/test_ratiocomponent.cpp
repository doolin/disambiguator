#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>
#include <clusterinfo.h>
#include <training.h>
#include <record.h>
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
    ft->load_fake_data(filename);
    RecordIndex * uid_dict = ft->get_uid_dict();
    //std::cout << "Size uid_dict: " << (*uid_dict).size() << std::endl;
    //rc = new cRatioComponent(*uid_dict, string("Patent"));
    rc = new cRatioComponent(*uid_dict, string("Personal"));
  }

 ~RatioComponentTest() {
    describe_test(INDENT2, "Destroying RatioComponentTest.");
    delete ft;
  }

  void load_fake_data() {
    const string filename("testdata/clustertest.csv");
    describe_test(INDENT2, "From load_fake_data in RatioComponentTest.");
    ft->load_fake_data(filename);
  }


  void test_sp_stats() {

    describe_test(INDENT2, "From test_sp_stats in RatioComponentTest.");

    //std::cout << "Size uid_dict: " << (*(rc->puid_tree)).size() << std::endl;
    //RecordIndex::const_iterator ri = rc->puid_tree->begin();
    //const Record * r = ri->second;
    //std::cout << "Record at 07625697-1:\n";
    //r->print();
    //std::cout << std::endl;

    TrainingPairs x_list, m_list;

    // TODO: Use this for testing exception.
    //x_list.push_back(TrainingPair("09625697-1", "07099871-3"));

    x_list.push_back(TrainingPair("07625697-1", "07099871-3"));
    m_list.push_back(TrainingPair("07013303-3", "07099871-3"));
    m_list.push_back(TrainingPair("07013303-3", "07099871-3"));
    m_list.push_back(TrainingPair("06453319-6", "06292880-6"));
    rc->get_similarity_info();

    rc->x_counts.clear();
    rc->m_counts.clear();

    rc->sp_stats(m_list, rc->m_counts);
    //std::cout << "Number of different match profiles (m_counts): " << rc->m_counts.size() << std::endl;
    CPPUNIT_ASSERT(1 == rc->m_counts.size());
    SPCountsIndex::const_iterator mc = rc->m_counts.begin();
    CPPUNIT_ASSERT(3 == mc->second);

    rc->sp_stats(x_list, rc->x_counts);
    //std::cout << "Number of different non-match profiles (x_counts): " << rc->x_counts.size() << std::endl;
    CPPUNIT_ASSERT(1 == rc->x_counts.size());
    SPCountsIndex::const_iterator xc = rc->x_counts.begin();
    CPPUNIT_ASSERT(1 == xc->second);


    //std::cout << "m_count value: " << (*c).second << std::endl;
    //print_similarity((*c).first);
    //print_similarity_profile_size((*c).first);
  }


  void test_create_ratios() {

    describe_test(INDENT2, "From test_create_ratios in RatioComponentTest.");

    // create training pairs
    // Create x_counts and m_counts
    // maybe clear ratio_map.
    rc->create_ratios();
    //std::cout << "Size of ratio_map: " << rc->ratio_map.size() << std::endl;
    // Find a way to test the result of that call.
  }

  void runTest() {
    //load_fake_data();
    test_sp_stats();
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
