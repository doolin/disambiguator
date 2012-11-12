#include <string>
#include <vector>

//#include <cppunit/Portability.h>
//#include <cppunit/portability/CppUnitSet.h>
//#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <ratios.h>
#include <cluster.h>

#include "testdata.h"
#include "testutils.h"
#include "fake.h"

class ClusterTest : public CppUnit::TestCase {

private:

  FakeTest * ft;
  vector<const Record *> rpv;


public:
  ClusterTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());

    const string filename("testdata/clustertest.csv");
    //const string filename("testdata/assignee_comparison.csv");
    ft = new FakeTest(string("Fake ClusterTest"), filename);
    ft->load_fake_data(filename);
    rpv = ft->get_recvecs();
  }

  void create_cluster() {
    Attribute::register_class_names(get_column_names());
    Record * r = make_quuxalot_record();
    ClusterHead ch(r, 0.9953);
    RecordPList rl = get_record_list();
    cBlocking_Operation_By_Coauthors  blocker_coauthor = get_blocker_coathor();
    Cluster::set_reference_patent_tree_pointer( blocker_coauthor.get_patent_tree());
    Cluster * c = new Cluster(ch, rl);
    delete c;
  }


  void test_find_representatives() {

    static const string useful_columns[] = {
      cFirstname::static_get_class_name(),
      cMiddlename::static_get_class_name(),
      cLastname::static_get_class_name(),
      cLatitude::static_get_class_name(),
      cAssignee::static_get_class_name(),
      cCity::static_get_class_name(),
      cCountry::static_get_class_name()
    };
    static const uint32_t numcols = sizeof(useful_columns)/sizeof(string);

    vector<uint32_t> indice = make_indice(useful_columns,numcols);

    Spec spec;
    spec.it("Indexing for find_representatives", [&indice](Description desc)->bool {
        vector<uint32_t> test = { 0, 2, 1, 7, 4, 10, 11 };
        return (test == indice);
    });


    const Record * r1 = rpv[1];
    const Record * r2 = rpv[2];
    const Record * r3 = rpv[3];

    typedef std::pair<const Attribute *, uint32_t> AttCount;
    typedef map<const Attribute *, uint32_t> AttCounts;
    RecordPList m_fellows = { r1, r2, r3 };
    vector<AttCounts> trace = make_trace(indice, m_fellows, numcols);

#if 0 /// Save all this it's really useful
    for_each(begin(trace), end(trace), [](AttCounts acs)->void {
        for_each(begin(acs), end(acs), [](AttCount ac)->void {
           const vector<const string*> & data = ac.first->get_data();
           for_each(begin(data), end(data), [](const string * s)->void {
              std::cout << "Att: " << s->c_str() << ", ";
           });
           std::cout << "count: " << ac.second << std::endl;
        });
    });
#endif

    vector<const Attribute *> most = get_most(trace, numcols);

    const Record * mp = get_record_with_most(most, m_fellows, indice, numcols);

    //mp->print();

    spec.it("Unique record ID for cluster representative should be 06453319-4", [mp](Description desc)->bool {
        const string uid = mp->get_unique_record_id();
        return (uid == string("06453319-4"));
    });

  }


  void runTest() {
    test_find_representatives();
    //create_cluster();
  }
};


void
test_clusters() {

  ClusterTest * rt = new ClusterTest(std::string("Cluster unit testing"));
  rt->runTest();
  delete rt;
}


#ifdef test_cluster_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusters();
  return 0;
}
#endif
