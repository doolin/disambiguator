
#include <string>
#include <vector>

//#include <cppunit/Portability.h>
//#include <cppunit/portability/CppUnitSet.h>
//#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>

#include "testdata.h"


class ClusterInfoTest : public CppUnit::TestCase {

public:
  ClusterInfoTest(std::string name) : CppUnit::TestCase(name) {}

  void create_cluster() {
    Attribute::register_class_names(get_column_names());
    Record * r = make_quuxalot_record();
    //ClusterInfoHead ch(r, 0.9953);
    RecordPList rl = get_record_list();
    cBlocking_Operation_By_Coauthors  blocker_coauthor = get_blocker_coathor();
    //ClusterInfo::set_reference_patent_tree_pointer( blocker_coauthor.get_patent_tree());
    //ClusterInfo * c = new ClusterInfo(ch, rl);
    //delete c;
  }


  void runTest() {
    create_cluster();
  }
};


void
test_clusters() {

  ClusterInfoTest * rt = new ClusterInfoTest(std::string("initial test"));
  rt->runTest();
  delete rt;
}


#ifdef test_clusterinfo_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusters();
  return 0;
}
#endif
