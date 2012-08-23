
#include <cppunit/TestCase.h>

#include <record.h>
#include <clusterhead.h>

#include "testdata.h"

class ClusterHeadTest : public CppUnit::TestCase { 

public:
  ClusterHeadTest(std::string name) : CppUnit::TestCase(name) {}

  void create_clusterhead() {
    Record * r = make_foobar_record();
    ClusterHead ch(r, 0.9953);
    CPPUNIT_ASSERT (0.9953 == ch.m_cohesion);
    delete r;
  }

  void runTest() {
    create_clusterhead();
  }
};


void
test_clusterhead() {

  ClusterHeadTest * cht = new ClusterHeadTest(std::string("initial test"));
  cht->runTest();
  delete cht;
}


#ifdef test_clusterhead_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_clusterhead();
  return 0;
}
#endif
