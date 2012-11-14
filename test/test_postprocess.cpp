
#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>
#include <clusterinfo.h>
#include <training.h>
#include <ratios.h>
#include <postprocess.h>

#include "testdata.h"
#include "testutils.h"
#include "fake.h"



class PostProcessTest : public CppUnit::TestCase {

private:

  FakeTest * ft;
  RecordPList recpointers;
  vector<const Record *> rpv;
  list<Record> all_records;


public:

  PostProcessTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  void runTests() {
  }

};


void
test_postprocess() {

  PostProcessTest * ppt = new PostProcessTest(std::string("Postprocess test"));
  ppt->runTests();
  delete ppt;
}


#ifdef test_postprocess_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_postprocess();
  return 0;
}
#endif
