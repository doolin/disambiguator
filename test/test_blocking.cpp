/** @file */
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


#include <disambiguate.h>
#include <attribute.h>
#include <cluster.h>
#include <newcluster.h>
#include <record.h>
#include <engine.h>
#include <blocking_operation.h>
#include "fake.h"

#include "testutils.h"


class BlockingTest : public CppUnit::TestCase {

private:
  FakeTest * ft;
  RecordPList recpointers;
  const cBlocking_Operation_By_Coauthors * cob;

public:
  BlockingTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
    const string filename("testdata/clustertest.csv");
    ft = new FakeTest(string("Fake RatioComponentTest"), filename);
    ft->load_fake_data(filename);
    recpointers = ft->get_recpointers();
    cob = ft->get_coauthor_blocking();
   }


  void test_read_blocking_config() {
    std::ofstream devnull("/dev/null");
    int result;
    result = BlockingConfiguration::config_blocking("./fixtures/BlockingConfig.txt", string("Round 1"), devnull);
    CPPUNIT_ASSERT(0 == result);
    describe_pass(INDENT2, "Reads a valid blocking file correctly");
  }

  void test_multi_column_blocking() {
#if 0
    const vector<const StringManipulator *> & inputvsm;
    const vector<string> & columnnames;
    iconst vector<unsigned int> & di;
    cBlocking_Operation_Multiple_Column_Manipulate & blocker_ref =
      cBlocking_Operation_Multiple_Column_Manipulate & blocker_ref();
#endif
  }

 /**
  * Coauthor blocking looks like a real pain, so letting this ride for now.
  * Will return to it later.
  */
  void test_get_topN_coauthors() {
    const Record * prec = recpointers.front();
    std::cout << "front pointer: " << recpointers.front() << std::endl;
    prec->print();
    //cob->get_topN_coauthors(prec, 2);
  }

  void test_coauthor_blocking() {
    //const Record * prec = recpointers.front();
    std::cout << "front pointer: " << recpointers.front() << std::endl;
    //std::string bi = cob->extract_blocking_info(recpointers.front());
    //std::cout << "blocking info: " << bi << std::endl;
  }

  void runTest() {
    test_read_blocking_config();
    test_multi_column_blocking();
    //test_get_topN_coauthors();
    //test_coauthor_blocking();
  }
};


void
test_blocking() {

  BlockingTest * bt = new BlockingTest(std::string("Blocking test..."));
  bt->runTest();
  delete bt;
}



#ifdef test_blocking_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_blocking();
  return 0;
}
#endif
