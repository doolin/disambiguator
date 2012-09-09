#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>

#include "record.h"

#include <record_private.h>

class RecordTest : public CppUnit::TestCase {

public:
  RecordTest(std::string name) : CppUnit::TestCase(name) {}

  Record make_foobar_record() {

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
    //std::cout << "Record..." << std::endl;
    return tmprec;
  }


  Record make_quuxalot_record() {

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
    //std::cout << "Record..." << std::endl;
    return tmprec;
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

  void test_get_data_by_index() {
    Record foobar = make_foobar_record();
    Record quux = make_quuxalot_record();

    const vector<const string *> data = quux.get_data_by_index(1);
    vector<const string *>::const_iterator it = data.begin();

    // Ok, so there's no data to print. Some other things have to
    // happen first.
    for (; it != data.end(); ++it) {
      std::cout << "data: " << *it << std::endl;
    }
  }

  void test_parse_column_names() {

    string names("Lastname,Firstname");
    vector<string> columns = parse_column_names(names);

    CPPUNIT_ASSERT(string("Lastname") == columns[0]);
    CPPUNIT_ASSERT(string("Firstname") == columns[1]);
  }


  void test_create_column_indices() {

    vector<string> requested_columns;
    requested_columns.push_back("Firstname");
    requested_columns.push_back("Lastname");
    requested_columns.push_back("Middlename");

    vector<string> total_col_names;
    total_col_names.push_back("Lastname");
    total_col_names.push_back("Firstname");
    total_col_names.push_back("Middlename");

    vector<uint32_t> rci =  create_column_indices(requested_columns, total_col_names);

    CPPUNIT_ASSERT(1 == rci[0]);
    CPPUNIT_ASSERT(0 == rci[1]);
    CPPUNIT_ASSERT(2 == rci[2]);

  }

  void runTest() {
    // Just o get startes...
    CPPUNIT_ASSERT( 1  == 1 );
    delete_record();
    make_foobar_record();
    make_quuxalot_record();
    attribute_record();
    read_records();
    test_get_data_by_index();
    test_create_column_indices();
    test_parse_column_names();
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
