
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

#define TESTCOLOR COLOR119
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
    char buffer[256];
    char teststr[] = "Comparing %s with %s, similarity %d";
    uint32_t similarity;

#if 0
  Totally different: THOMAS ERIC/RICHARD JACK EVAN
  ONE NAME MISSING: THOMAS ERIC/(NONE)
  THOMAS ERIC/ THOMAS JOHN ALEX
  LEE RON ERIC/LEE ALEX ERIC
  No space match but raw names do not: JOHNERIC/JOHN ERIC.
  Short name vs long name: ERIC/ERIC THOMAS
  ALEX NICHOLAS/ALEX NICHOLAS TAKASHI
  ALEX NICHOLAS/ALEX NICHOLA (Might be not exactly the same but identified the same by jaro-wrinkler)
#endif

    string n1s("THOMAS ERIC");
    cFirstname n1(n1s.c_str());
    n1.split_string(n1s.c_str());

    string n2s("RICHARD JACK EVAN");
    cFirstname n2(n2s.c_str());
    n2.split_string(n2s.c_str());

    n1.activate_comparator();

    similarity = n1.compare(n2);
    sprintf(buffer, teststr, n1s.c_str(), n2s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    string n3s("");
    cFirstname n3(n3s.c_str());
    n3.split_string(n3s.c_str());

    similarity = n1.compare(n3);
    sprintf(buffer, teststr, n1s.c_str(), n3s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

    string n4s("LEE RON ERIC");
    cFirstname n4(n4s.c_str());
    n4.split_string(n4s.c_str());

    string n5s("LEE ALEX ERIC");
    cFirstname n5(n5s.c_str());
    n5.split_string(n5s.c_str());

    similarity = n4.compare(n5);
    sprintf(buffer, teststr, n4s.c_str(), n5s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    string n6s("JOHNERIC");
    cFirstname n6(n6s.c_str());
    n6.split_string(n6s.c_str());

    string n7s("JOHN ERIC");
    cFirstname n7(n7s.c_str());
    n7.split_string(n7s.c_str());

    similarity = n6.compare(n7);
    sprintf(buffer, teststr, n6s.c_str(), n7s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(2 == similarity);

    string n8s("ERIC");
    cFirstname n8(n8s.c_str());
    n8.split_string(n8s.c_str());

    string n9s("ERIC THOMAS");
    cFirstname n9(n9s.c_str());
    n9.split_string(n9s.c_str());

    similarity = n8.compare(n9);
    sprintf(buffer, teststr, n8s.c_str(), n9s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    string n10s("ALEX NICHOLAS");
    cFirstname n10(n10s.c_str());
    n10.split_string(n10s.c_str());

    string n11s("ALEX NICHOLAS TAKASHI");
    cFirstname n11(n11s.c_str());
    n11.split_string(n11s.c_str());

    similarity = n10.compare(n11);
    sprintf(buffer, teststr, n10s.c_str(), n11s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    string n12s("ALEX NICHOLA");
    cFirstname n12(n12s.c_str());
    n12.split_string(n12s.c_str());

    similarity = n10.compare(n12);
    sprintf(buffer, teststr, n10s.c_str(), n12s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

  }


  // TODO: Maybe consider moving this to it's own file.
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

    similarity = e5.compare(e6);
    sprintf(buffer, teststr, e5s.c_str(), e6s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    string e7s("THOMAS ERIC RON ALEX EDWARD");
    cMiddlename e7(e7s.c_str());
    e7.split_string(e7s.c_str());

    string e8s("JACK ERIC RON ALEX LEE");
    cMiddlename e8(e8s.c_str());
    e8.split_string(e8s.c_str());

    similarity = e7.compare(e8);
    sprintf(buffer, teststr, e7s.c_str(), e8s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    string e9s("THOMAS ERIC LEE");
    cMiddlename e9(e9s.c_str());
    e9.split_string(e9s.c_str());

    similarity = e1.compare(e9);
    sprintf(buffer, teststr, e1s.c_str(), e9s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(3 == similarity);


    delete foobar;
  }

  void compare_lastname() {

    describe_test(INDENT2, "Testing Lastname comparison");

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);
    uint32_t similarity;
    char buffer[256];
    char teststr[] = "Comparing %s with %s, similarity %d";

#if 0
Totally different names:
  ANDERSON/DAVIDSON
  One name missing: ANDERSON/(NONE)
  First part of name doesn’t match: DE AMOUR/DA AMOUR
  VAN DE WAALS/VAN DES WAALS
  DE AMOUR/DEAMOUR
  JOHNSTON/JOHNSON
  DE AMOUR/DE AMOURS
#endif

    string l1s("ANDERSON");
    cLastname l1(l1s.c_str());
    l1.split_string(l1s.c_str());
    l1.activate_comparator();

    string l2s("DAVIDSON");
    cLastname l2(l2s.c_str());
    l2.split_string(l2s.c_str());

    similarity = l1.compare(l2);
    sprintf(buffer, teststr, l1s.c_str(), l2s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(1 == similarity);

    string l3s("");
    cLastname l3(l3s.c_str());
    l3.split_string(l3s.c_str());

    similarity = l1.compare(l3);
    sprintf(buffer, teststr, l1s.c_str(), l3s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(0 == similarity);

    string l4s("DE AMOUR");
    cLastname l4(l4s.c_str());
    l4.split_string(l4s.c_str());

    string l5s("DA AMOUR");
    cLastname l5(l5s.c_str());
    l5.split_string(l5s.c_str());

    similarity = l4.compare(l5);
    sprintf(buffer, teststr, l4s.c_str(), l5s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(3 == similarity);

    //VAN DE WAALS/VAN DES WAALS
    string l6s("VAN DE WAALS");
    cLastname l6(l6s.c_str());
    l6.split_string(l6s.c_str());

    string l7s("VAN DES WAALS");
    cLastname l7(l7s.c_str());
    l7.split_string(l7s.c_str());

    similarity = l6.compare(l7);
    sprintf(buffer, teststr, l6s.c_str(), l7s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    //DE AMOUR/DEAMOUR
    string l8s("DE AMOUR");
    cLastname l8(l8s.c_str());
    l8.split_string(l8s.c_str());

    string l9s("DEAMOUR");
    cLastname l9(l9s.c_str());
    l9.split_string(l9s.c_str());

    similarity = l8.compare(l9);
    sprintf(buffer, teststr, l8s.c_str(), l9s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    //JOHNSTON/JOHNSON
    string l10s("JOHNSTON");
    cLastname l10(l10s.c_str());
    l10.split_string(l10s.c_str());

    string l11s("JOHNSON");
    cLastname l11(l11s.c_str());
    l11.split_string(l11s.c_str());

    similarity = l10.compare(l11);
    sprintf(buffer, teststr, l10s.c_str(), l11s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    //DE AMOUR/DE AMOURS
    string l12s("DE AMOUR");
    cLastname l12(l12s.c_str());
    l12.split_string(l12s.c_str());

    string l13s("DE AMOURS");
    cLastname l13(l13s.c_str());
    l13.split_string(l13s.c_str());

    similarity = l12.compare(l13);
    sprintf(buffer, teststr, l12s.c_str(), l13s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(4 == similarity);

    similarity = l12.compare(l12);
    sprintf(buffer, teststr, l12s.c_str(), l12s.c_str(), similarity);
    describe_test(INDENT4, buffer);
    CPPUNIT_ASSERT(5 == similarity);


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
