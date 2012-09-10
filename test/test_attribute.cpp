
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <string>

// Really good web pages:
// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
// http://stackoverflow.com/questions/7182359/template-instantiation-details-of-gcc-and-ms-compilers
#include <disambiguation.h>
#include <engine.h>
#include <attribute.h>

#include "testdata.h"

using std::string;
using std::cout;
using std::endl;

class AttributeTest : public CppUnit::TestCase {

public:
  AttributeTest(std::string name) : CppUnit::TestCase(name) {}


  void compare_exact() {

    Record * foobar = make_foobar_record();
    foobar->set_sample_record(foobar);

    cFirstname d1("Dave");
    cout << "1" << endl;
    d1.split_string("Dave");
    cout << "2" << endl;
    d1.activate_comparator();
    cout << "3" << endl;
    cFirstname d2("Dave");
    cout << "4" << endl;
    d2.split_string("Dave");
    cout << "5" << endl;
    d2.activate_comparator();
    cout << "6" << endl;
    uint32_t similarity = d1.compare(d2);
    cout << "7" << endl;
    std::cout << "Similarity: " << similarity << std::endl;
    cout << "8" << endl;
    similarity = d1.exact_compare(d1);
    cout << "9" << endl;
    std::cout << "Similarity: " << similarity << std::endl;
  }


  // Test for memory leakage using valgrind
  void delete_attribute() {
    cFirstname * a = new cFirstname("Dave");
    delete a;
  }

  void runTest() {
    compare_exact();
    delete_attribute();
  }
};


int
main(int UP(argc), char ** UP(argv)) {

  AttributeTest * at = new AttributeTest(std::string("initial test"));
  at->runTest();
  delete at;
  return 0;
}
