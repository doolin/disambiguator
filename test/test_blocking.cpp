/** @file */
#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <attribute.h>
#include <strcmp95.h>
#include <txt2sqlite3.h>

#include "testutils.h"


class BlockingTest : public CppUnit::TestCase {

public:
  BlockingTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
  }

  void delete_blocking() {
    //cBlocking * rc = new cBlocking();
    //delete rc;
  }

  void runTest() {
    delete_blocking();
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
