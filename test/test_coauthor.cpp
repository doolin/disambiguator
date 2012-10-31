#include <string>
#include <string.h>
#include <cppunit/TestCase.h>

// Really good web pages:
// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
// http://stackoverflow.com/questions/7182359/template-instantiation-details-of-gcc-and-ms-compilers
#include <disambiguation.h>
#include <record.h>
#include <ratios.h>
#include <engine.h>
#include <newcluster.h>
#include <attribute.h>

#include "testdata.h"
#include "colortest.h"
#include "testutils.h"
#include "fake.h"

using std::string;
using std::cout;
using std::endl;


class CoauthorTest : public CppUnit::TestCase {

private:
  FakeTest * ft;
  RecordPList rp;
  static const short BUF_SIZE = 256;

public:

  CoauthorTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
    ft = new FakeTest("FakeTest for assignees", "./testdata/assignee_comparison.csv");
    ft->load_fake_data("./testdata/assignee_comparison.csv");
    rp = ft->get_recpointers();

  }


  void test_r1r2() {

    RecordPList rp = ft->get_recpointers();
    vector<const Record *> rpv = ft->get_recvecs();
    const Record & r1 = *rpv[1];
    const Record & r2 = *rpv[1];

    vector<string> active_similarity_attributes;
    active_similarity_attributes.push_back(string("Coauthor"));
    Record::activate_comparators_by_name(active_similarity_attributes);

    SimilarityProfile sp = r1.record_compare(r2);
    uint32_t similarity = sp[0];
    //CPPUNIT_ASSERT(6 == similarity);
    describe_pass(INDENT4, "Comparing similarity for r1 and r2");
    print_similarity(sp);
  }


  void test_coauthors() {
  }


};


void
test_fetch_records() {

  CoauthorTest * ct = new CoauthorTest("Testing assignee_comparison");
  ct->test_r1r2();
  ct->test_coauthors();
  delete ct;
}


#ifdef test_coauthor_STANDALONE
int
main(int, char **) {

  test_fetch_records();

  return 0;
}
#endif
