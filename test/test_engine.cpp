
#include <cppunit/TestCase.h>

#include <engine.h>

#include "testdata.h"

class EngineTest : public CppUnit::TestCase {

private:
  static const std::string LINE;
  std::vector<std::string> tcn;
  std::vector<std::string> requested_columns;
  std::vector<unsigned int> indices;

public:
  EngineTest(std::string name) : CppUnit::TestCase(name) {}

  void set_up() {
    tcn = parse_column_names(LINE);
    requested_columns = get_column_names();
    indices = create_column_indices(requested_columns, tcn);
    Attribute::register_class_names(requested_columns);
  }

  void test_parse_column_names() {
    CPPUNIT_ASSERT(9 == tcn.size());
  }

  void test_create_column_indices() {
    CPPUNIT_ASSERT(9 == indices.size());
  }

  void test_instantiate_attributes() {
    Attribute ** pointer_array = new Attribute*[tcn.size()];
    pointer_array = instantiate_attributes(tcn, tcn.size());
    // TODO: Add unit test code of some sort here.
    delete pointer_array[0];
    for (unsigned int i = 1; i< tcn.size(); ++i) {
      delete pointer_array[i];
    }
    delete [] pointer_array;
  }


  void runTest() {
    set_up();
    test_parse_column_names();
    test_create_column_indices();
    test_instantiate_attributes();
  }
};

const std::string EngineTest::LINE  = "Firstname,Middlename,Lastname,Latitude,Assignee,City,Country,Patent,ApplyYear";

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
