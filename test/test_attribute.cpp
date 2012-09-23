
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>
#include <string.h>

// Really good web pages:
// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
// http://stackoverflow.com/questions/7182359/template-instantiation-details-of-gcc-and-ms-compilers
#include <disambiguation.h>
#include <engine.h>
#include <attribute.h>

#include "testdata.h"
#include "colortest.h"

#define TESTCOLOR COLOR181
#define INDENT0 ""
#define INDENT2 "  "
#define INDENT4 "     "

using std::string;
using std::cout;
using std::endl;

void
describe_test(const char * indent, const char * description) {

    std::cout << indent << TESTCOLOR << description << COLOR_RESET
              << std::endl;
}

class TestUtils {
};

class AttributeTest : public CppUnit::TestCase, TestUtils {

public:
  AttributeTest(std::string name) : CppUnit::TestCase(name) {
    describe_test(INDENT0, "Testing Attribute");
  }


  void compare_firstname() {

    describe_test(INDENT2, "Testing Firstname comparison");

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);

    cFirstname d1("Dave");
    d1.split_string("Dave");
    cFirstname d2("Dave");
    d2.split_string("Dave");
    d1.activate_comparator();
    //d2.activate_comparator();
    uint32_t similarity = d1.compare(d2);
    //std::cout << "Similarity: " << similarity << std::endl;
    CPPUNIT_ASSERT(4 == similarity);
    similarity = d1.exact_compare(d1);
    //std::cout << "Similarity: " << similarity << std::endl;
  }


  void compare_middlename() {

    describe_test(INDENT2, "Testing Middlename comparison");

    char buffer[256];
    char teststr[] = "Comparing %s with %s, similarity %d";

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;

    string d1s("David");
    cMiddlename d1(d1s.c_str());
    d1.split_string(d1s.c_str());
    string d2s("David");
    cMiddlename d2(d2s.c_str());
    d2.split_string(d2s.c_str());
    d1.activate_comparator();

    similarity = d1.compare(d2);
    sprintf(buffer, teststr, d1s.c_str(), d2s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(2 == similarity);


    string d3s("David Michael");
    string d4s("David Michael");
    cMiddlename d3(d3s.c_str());
    d3.split_string(d3s.c_str());
    cMiddlename d4(d4s.c_str());
    d4.split_string(d4s.c_str());

    similarity = d3.compare(d4);
    sprintf(buffer, teststr, d3s.c_str(), d4s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(3 == similarity);

    similarity = d1.compare(d3);
    sprintf(buffer, teststr, d1s.c_str(), d3s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

    similarity = d1.compare(d4);
    d1.get_data();
    sprintf(buffer, teststr, d1s.c_str(), d4s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);


    string d5s("Anna Yvette");
    cMiddlename d5(d5s.c_str());
    d5.split_string(d5s.c_str());
    similarity = d1.compare(d5);
    sprintf(buffer, teststr, d1s.c_str(), d5s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

    string d6s("AnnaYvette");
    cMiddlename d6("AnnaYvette");
    d6.split_string("AnnaYvette");
    similarity = d1.compare(d6);
    sprintf(buffer, teststr, d1s.c_str(), d6s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(2 == similarity);

    string d7s("Anna");
    cMiddlename d7(d7s.c_str());
    d7.split_string(d7s.c_str());


    string d8s("Yvette");
    cMiddlename d8(d8s.c_str());
    d8.split_string(d8s.c_str());
    string d9s("");
    cMiddlename d9(d9s.c_str());
    d9.split_string(d9s.c_str());
    similarity = d8.compare(d9);
    sprintf(buffer, teststr, d8s.c_str(), d9s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(2 == similarity);


    string d10s("David Kenneth");
    cMiddlename d10(d10s.c_str());
    d10.split_string(d10s.c_str());
    similarity = d4.compare(d10);
    sprintf(buffer, teststr, d4s.c_str(), d10s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    similarity = d5.compare(d9);
    sprintf(buffer, teststr, d5s.c_str(), d9s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

#if 0
   THOMAS ERIC/JOHN THOMAS
   JOHN ERIC/JOHN (MISSING)
   THOMAS ERIC ALEX/JACK ERIC RONALD
   THOMAS ERIC RON ALEX EDWARD/JACK ERIC RON ALEX LEE
   THOMAS ERIC/THOMAS ERIC LEE
#endif

    // This is from the previous documentation, in which
    // middlenames run from 0..4. Now, Middlename compares
    // run from 0..3

    string e1s("THOMAS ERIC");
    cMiddlename e1(e1s.c_str());
    e1.split_string(e1s.c_str());

    string e2s("JOHN THOMAS");
    cMiddlename e2(e2s.c_str());
    e2.split_string(e2s.c_str());

    similarity = e1.compare(e2);
    sprintf(buffer, teststr, e1s.c_str(), e2s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    string e3s("JOHN ERIC");
    cMiddlename e3(e3s.c_str());
    e3.split_string(e3s.c_str());

    string e4s("JOHN");
    cMiddlename e4(e4s.c_str());
    e4.split_string(e4s.c_str());

    similarity = e3.compare(e4);
    sprintf(buffer, teststr, e3s.c_str(), e4s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

    string e5s("THOMAS ERIC ALEX");
    cMiddlename e5(e5s.c_str());
    e5.split_string(e5s.c_str());

    string e6s("JACK ERIC RONALD");
    cMiddlename e6(e6s.c_str());
    e6.split_string(e6s.c_str());

    string e7s("THOMAS ERIC RON ALEX EDWARD");
    cMiddlename e7(e7s.c_str());
    e7.split_string(e7s.c_str());

    string e8s("JACK ERIC RON ALEX LEE");
    cMiddlename e8(e8s.c_str());
    e8.split_string(e8s.c_str());

    string e9s("THOMAS ERIC LEE");
    cMiddlename e9(e9s.c_str());
    e9.split_string(e9s.c_str());


    delete foobar;
  }

  void compare_lastname() {

    describe_test(INDENT2, "Testing Lastname comparison");

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;

    cLastname d1("Williams");
    d1.split_string("Williams");
    cLastname d2("Wilson");
    d2.split_string("Wilson");

    d1.activate_comparator();

    similarity = d1.compare(d2);
    std::cout << "Lastname similarity d1, d2: " << similarity << std::endl;
    //CPPUNIT_ASSERT(4 == similarity);

    delete foobar;
  }


  void compare_assignee() {

    describe_test(INDENT2, "Testing Assignee comparison");

    RecordPList all_rec_pointers;

    // This is necessary God object.
    // TODO: Remind myself why this object needs creating
    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;

    cAssignee d1("IBM");
    d1.split_string("IBM");
    cAssignee d2("IBM");
    d2.split_string("IBM");

    d1.activate_comparator();
    d1.configure_assignee(all_rec_pointers);

    similarity = d1.compare(d2);
    std::cout << "Assignee similarity d1, d2: " << similarity << std::endl;
    //CPPUNIT_ASSERT(4 == similarity);

    delete foobar;
  }


  void compare_coauthor() {

    describe_test(INDENT2, "Testing Coauthor comparison");

#if 0
    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;
    cCoauthor d1("IBM");
    d1.split_string("IBM");
    cCoauthor d2("IBM");
    d2.split_string("IBM");

    d1.activate_comparator();

    similarity = d1.compare(d2);
    std::cout << "Coauthor similarity d1, d2: " << similarity << std::endl;
    //CPPUNIT_ASSERT(4 == similarity);

    delete foobar;
#endif

  }


  void compare_class() {

    describe_test(INDENT2, "Testing Class comparison");

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;

    cClass d1;
    d1.split_string("128/606/623");
    cClass d2;
    d2.split_string("128/606/624");

    d1.activate_comparator();

    similarity = d1.compare(d2);
    std::cout << "Class similarity d1, d2: " << similarity << std::endl;
    //CPPUNIT_ASSERT(4 == similarity);

    delete foobar;

  }


  void compare_distance() {

    describe_test(INDENT2, "Testing Distance comparison");

#if 0
    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;

    cLatitude d1("IBM");
    d1.split_string("IBM");
    cLatitude d2("IBM");
    d2.split_string("IBM");

    d1.activate_comparator();

    similarity = d1.compare(d2);
    std::cout << "Latitude similarity d1, d2: " << similarity << std::endl;
    //CPPUNIT_ASSERT(4 == similarity);

    delete foobar;
#endif

  }


  // Test for memory leakage using valgrind
  void delete_attribute() {
    cFirstname * a = new cFirstname("Dave");
    delete a;
  }


  void runTest() {
    compare_firstname();
    compare_middlename();
    compare_lastname();
    //compare_assignee();
    compare_class();
    compare_coauthor();
    compare_distance();
    delete_attribute();
  }
};

#ifdef test_attribute_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  AttributeTest * at = new AttributeTest(std::string("initial test"));
  at->runTest();
  delete at;
  return 0;
}
#endif
