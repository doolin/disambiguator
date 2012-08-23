#include <string>
#include <vector>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>

#include "testdata.h"

class ClusterTest : public CppUnit::TestCase { 

public: 
  ClusterTest(std::string name) : CppUnit::TestCase(name) {}

  void attribute_record() {
    cFirstname * firstname  = new cFirstname("Foobar");
    std::string tmp("bar");

    firstname->reset_data(tmp.c_str());
    const Attribute * pAttrib;

    pAttrib = firstname->clone();

    vector <const Attribute *> temp_vec_attrib;
    temp_vec_attrib.push_back(pAttrib);
    Record tmprec(temp_vec_attrib);

    //Record(const vector <const Attribute *> & input_vec);
    //const std::vector <const Attribute *> input_vec;
    //input_vec.push_back(firstname);
    //Record r(input_vec);
    delete firstname;
  }

  void read_records() {
    CPPUNIT_ASSERT(1 == 1);
  }

  void delete_record() {
    Record * rc = new Record();
    delete rc;
  }

  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_record();
    make_foobar_record();
    make_quuxalot_record();
    attribute_record();
    read_records();
  }
};


void
test_clusters() {

  ClusterTest * rt = new ClusterTest(std::string("initial test"));
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
