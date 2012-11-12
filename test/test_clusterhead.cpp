
#include <cppunit/TestCase.h>

#include <record.h>
#include <clusterhead.h>

#include "testdata.h"
#include "testutils.h"

class ClusterHeadTest : public CppUnit::TestCase {

public:
  ClusterHeadTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  void create_clusterhead() {

    Spec spec;

    Record * representative = make_foobar_record();
    spec.it("Creates a ClusterHead", [representative](Description desc)->bool {
      double cohesion = 0.9953;
      ClusterHead ch(representative, cohesion);
      return (cohesion == ch.m_cohesion);
    });
    // Segfaults...
    //r->print();
    // segfaults, which shouldn't be...
    // Leaks, bad
    //r->clean_member_attrib_pool();
    //std::cout << "sizeof(r): " << sizeof(*r) << std::endl;
    delete representative;
  }

  void runTest() {
    create_clusterhead();
  }
};


void
test_clusterhead() {

  ClusterHeadTest * cht = new ClusterHeadTest(std::string("ClusterHead unit testing"));
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
