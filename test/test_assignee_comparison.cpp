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


class AssigneeComparisonTest : public CppUnit::TestCase {

private:
  FakeTest * ft;
  RecordPList rp;
  string H0008;

public:

  AssigneeComparisonTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, name.c_str());
    ft = new FakeTest("FakeTest for assignees", "./testdata/assignee_comparison.csv");
    ft->load_fake_data("./testdata/assignee_comparison.csv");
    rp = ft->get_recpointers();

    H0008 = string("H0008");
  }

  void print_assignee_comparison() {
  }


  void compare_assignee() {

    describe_test(INDENT2, "Testing Assignee comparison");

    uint32_t similarity;


    string asgnum("H0008");

    cAssignee d1("IBM");
    d1.split_string("IBM");
    cAsgNum asg1(H0008.c_str());
    asg1.split_string(H0008.c_str());
    vector<const Attribute *> atpv1;
    atpv1.push_back(&asg1);
    d1.config_interactive(atpv1);

    cAssignee d2("IBM");
    d2.split_string("IBM");
    cAsgNum asg2(asgnum.c_str());
    asg2.split_string(asgnum.c_str());
    vector<const Attribute *> atpv2;
    atpv2.push_back(&asg2);
    d2.config_interactive(atpv2);

    d1.activate_comparator();

    RecordPList rp = ft->get_recpointers();
    cAssignee::configure_assignee(rp);

    similarity = d1.compare(d2);
    //std::cout << "Assignee similarity d1, d2: " << similarity << std::endl;
    CPPUNIT_ASSERT(4 == similarity);
    describe_pass(INDENT4, "Comparing similarity for IBM");

  }


  void test_different_assignees() {

    cAssignee::configure_assignee(rp);
    cAssignee d2("IBM");
    d2.split_string("IBM");
    cAsgNum asg2(H0008.c_str());
    asg2.split_string(H0008.c_str());
    vector<const Attribute *> atpv2;
    atpv2.push_back(&asg2);
    d2.config_interactive(atpv2);


    cAssignee a3("International Harvester");
    a3.split_string("International Harvester");
    cAsgNum asg3("I77777");
    asg3.split_string("I77777");
    vector<const Attribute *> atpv3;
    atpv3.push_back(&asg3);
    a3.config_interactive(atpv3);

    a3.activate_comparator();


    uint32_t similarity = d2.compare(a3);
    std::cout << "Assignee similarity d1, d2: " << similarity << std::endl;
    CPPUNIT_ASSERT(4 == similarity);
    describe_fail(INDENT4, "Comparing similarity for IBM & International Harvester");

  }


  void test_r1r5() {

    RecordPList rp = ft->get_recpointers();
    cAssignee::configure_assignee(rp);
    vector<const Record *> rpv = ft->get_recvecs();
    const Record & r1 = *rpv[1];
    const Record & r2 = *rpv[1];

    vector<string> active_similarity_attributes;
    active_similarity_attributes.push_back(string("Assignee"));
    //Record::activate_comparators_by_name(BlockingConfiguration::active_similarity_attributes);
    Record::activate_comparators_by_name(active_similarity_attributes);

    SimilarityProfile sp = r1.record_compare(r2);
    print_similarity(sp);
  }

};


void
test_fetch_records() {

  AssigneeComparisonTest * act = new AssigneeComparisonTest("Testing assignee_comparison");
  //act->compare_assignee();
  //act->test_different_assignees();
  act->test_r1r5();
  delete act;
}


#ifdef test_assignee_comparison_STANDALONE
int
main(int, char **) {

  test_fetch_records();

  return 0;
}
#endif
