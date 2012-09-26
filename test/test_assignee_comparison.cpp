#include <string>
#include <string.h>
#include <cppunit/TestCase.h>

// Really good web pages:
// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
// http://stackoverflow.com/questions/7182359/template-instantiation-details-of-gcc-and-ms-compilers
//#include <disambiguation.h>
#include <engine.h>
//#include <attribute.h>

#include "testdata.h"
#include "colortest.h"
#include "testutils.h"


using std::string;
using std::cout;
using std::endl;


class AssigneeComparisonTest : public CppUnit::TestCase {

public:

  AssigneeComparisonTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
  }

  void test_get_records() {

    describe_test(INDENT2, "Testing fetch_records_from_txt...");

    list<Record> source;
    // filename is 20 bytes, check how it leaks.
    const char * filename = "testdata/assignee_comparison.csv";
    vector<string> requested_columns;
    requested_columns.push_back(string("Firstname"));
    requested_columns.push_back(string("Lastname"));
    requested_columns.push_back(string("Middlename"));
    requested_columns.push_back(string("Assignee"));
    requested_columns.push_back(string("AsgNum"));
    bool successful = fetch_records_from_txt(source, filename, requested_columns);

    if (not successful)
      exit(-1);
  }

  /**
   * Now, with a load of records, I should be able to test specific
   * records for attributes and values.
   */

};


void
test_fetch_records() {

  AssigneeComparisonTest * frt = new AssigneeComparisonTest("Testing assignee_comparison");
  frt->test_get_records();
  delete frt;
}


#ifdef test_assignee_comparison_STANDALONE
int
main(int, char **) {

  test_fetch_records();

  return 0;
}
#endif
