
#include <cppunit/TestCase.h>

#include <engine.h>



class EngineTest : public CppUnit::TestCase {

public:
  EngineTest(std::string name) : CppUnit::TestCase(name) {}

  void parse_total_column_names() {
    std::string line("Firstname,Middlename,Lastname,Latitude,Assignee,City,Country,Patent,ApplyYear");
    std::vector<std::string> tcn = parse_column_names(line);
    CPPUNIT_ASSERT(9 == tcn.size());
  }


  void runTest() {
    parse_total_column_names();
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
