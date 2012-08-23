#include <string>
#include <vector>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>

class RecordTest : public CppUnit::TestCase { 

public: 
  RecordTest(std::string name) : CppUnit::TestCase(name) {}

  void make_foobar_record() {

    cFirstname  * firstname   = new cFirstname("Foo");
    cMiddlename * middlename  = new cMiddlename("X");
    cLastname   * lastname    = new cLastname("Bar");
    cLatitude   * latitude    = new cLatitude("42.00");
    cAssignee   * assignee    = new cAssignee("Gonesilent");
    cCity       * city        = new cCity("Burlingame");
    cCountry    * country     = new cCountry("US");

    vector <const Attribute *> temp_vec_attrib;

    temp_vec_attrib.push_back(firstname);
    temp_vec_attrib.push_back(middlename);
    temp_vec_attrib.push_back(lastname);
    temp_vec_attrib.push_back(latitude);
    temp_vec_attrib.push_back(assignee);
    temp_vec_attrib.push_back(city);
    temp_vec_attrib.push_back(country);

    Record tmprec(temp_vec_attrib);
    std::cout << "Record..." << std::endl;
  }


  void make_quuxalot_record() {

    cFirstname  * firstname   = new cFirstname("Quux");
    cMiddlename * middlename  = new cMiddlename("A");
    cLastname   * lastname    = new cLastname("Lot");
    cLatitude   * latitude    = new cLatitude("42.00");
    cAssignee   * assignee    = new cAssignee("Gonesilent");
    cCity       * city        = new cCity("Burlingame");
    cCountry    * country     = new cCountry("US");

    vector <const Attribute *> temp_vec_attrib;

    temp_vec_attrib.push_back(firstname);
    temp_vec_attrib.push_back(middlename);
    temp_vec_attrib.push_back(lastname);
    temp_vec_attrib.push_back(latitude);
    temp_vec_attrib.push_back(assignee);
    temp_vec_attrib.push_back(city);
    temp_vec_attrib.push_back(country);

    Record tmprec(temp_vec_attrib);
    std::cout << "Record..." << std::endl;
  }


  void attribute_record() {
    cFirstname * firstname  = new cFirstname("Foobar");
    std::string tmp("bar");

    firstname->reset_data(tmp.c_str());
    const Attribute * pAttrib;

    pAttrib = firstname->clone();

    vector <const Attribute *> temp_vec_attrib;
    temp_vec_attrib.push_back(pAttrib);
    Record tmprec(temp_vec_attrib);

    //Record(const vector <const Attribute *> & input_vec);
    //const std::vector <const Attribute *> input_vec;
    //input_vec.push_back(firstname);
    //Record r(input_vec);
    delete firstname;
  }

  void read_records() {
    CPPUNIT_ASSERT(1 == 1);
  }

  void delete_record() {
    Record * rc = new Record();
    delete rc;
  }

  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_record();
    make_foobar_record();
    make_quuxalot_record();
    attribute_record();
    read_records();
  }
};


void
test_records() {

  RecordTest * rt = new RecordTest(std::string("initial test"));
  rt->runTest();
  delete rt;
}


#ifdef test_record_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_records();
  return 0;
}
#endif
