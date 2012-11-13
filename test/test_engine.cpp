#include <cstdio>

#include <cppunit/TestCase.h>

#include <engine.h>

#include "testdata.h"
#include "testutils.h"

using std::string;
using std::vector;

class EngineTest : public CppUnit::TestCase {

private:
  static const std::string LINE;
  vector<string> tcn;
  vector<string> requested_columns;
  vector<unsigned int> indices;

public:
  EngineTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

  void set_up() {
    tcn = parse_column_names(LINE);
    requested_columns = get_column_names();
    // TODO: Find out why this is failing with an exception
    indices = create_column_indices(requested_columns, tcn);
    Attribute::register_class_names(requested_columns);
  }

  void test_parse_column_names() {
    Spec spec;
    spec.it("Correctly parsed column names", DO_SPEC_THIS {
      return (10 == tcn.size());
    });
  }


  void test_create_column_indices() {
    Spec spec;
    spec.it("Indices have the correct length", DO_SPEC_THIS {
      //std::cout << "indices.size: " << indices.size() << std::endl;
      return (10 == indices.size());
    });
  }


  void test_instantiate_attributes() {

    Attribute ** pa = new Attribute*[tcn.size()];
    pa = instantiate_attributes(tcn, tcn.size());

    Spec spec;
    spec.it("First attribute has attrib_group 'Personal'", [this,pa](Description desc)->bool {
      return (string("Personal") == pa[0]->get_attrib_group());
    });

    delete pa[0];
    for (unsigned int i = 1; i< tcn.size(); ++i) {
      delete pa[i];
    }
    delete [] pa;
  }


  void runTest() {
    set_up();
    test_parse_column_names();
    test_create_column_indices();
    test_instantiate_attributes();
  }
};

const std::string EngineTest::LINE  = "Firstname,Middlename,Lastname,Latitude,Assignee,City,Country,Patent,ApplyYear,AsgNum";

void
test_engine() {

  EngineTest * et = new EngineTest(std::string("Engine unit test"));
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
