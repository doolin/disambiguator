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


class FetchRecordTest : public CppUnit::TestCase {

public:

  FetchRecordTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
  }

  void test_get_records() {

    list<Record> source;
    const char * filename = "testdata/invpat2.txt";
    vector<string> requested_columns;
    requested_columns.push_back(string("Firstname"));
    bool is_success = fetch_records_from_txt(source, filename, requested_columns);
  }

};


void
test_fetch_records() {

  FetchRecordTest * frt = new FetchRecordTest("Testing fetch_records");
  frt->test_get_records();
  delete frt;

}

#ifdef test_fetch_records_STANDALONE
int
main(int, char **) {

  test_fetch_records();

  return 0;
}
#endif
