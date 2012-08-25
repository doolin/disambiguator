#include <cstdio>
#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <training.h>

#include "colortest.h"

class TrainingTest : public CppUnit::TestCase { 

public: 
  TrainingTest(std::string name) : CppUnit::TestCase(name) {
    fprintf(stdout, "%s\n", name.c_str());
  }

  void delete_blocking() {
  }

  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_blocking();
  }
};


void
test_training() {

  TrainingTest * tt = new TrainingTest(std::string(COLOR124"Training initial test"COLOR_RESET));
  tt->runTest();
  delete tt;
}

#ifdef test_training_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_training();
  return 0;
}
#endif
