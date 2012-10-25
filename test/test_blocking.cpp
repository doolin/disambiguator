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
   }

  void test_coauthor_blocking() {
    cob = ft->get_coauthor_blocking();
    //std::string bi = cob->extract_blocking_info(recpointers.front());
    //std::cout << "blocking info: " << bi << std::endl;
  }

  void runTest() {
    test_coauthor_blocking();
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
