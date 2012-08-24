
#include <cppunit/TestCase.h>

#include <engine.h>



class EngineTest : public CppUnit::TestCase {

public:
  EngineTest(std::string name) : CppUnit::TestCase(name) {}

  void create_cluster() {
  }


  void runTest() {
    create_cluster();
  }
};


void
test_engine() {

  EngineTest * et = new EngineTest(std::string("initial test"));
  et->runTest();
  delete et;
}


#ifdef test_engine_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_engine();
  return 0;
}
#endif
