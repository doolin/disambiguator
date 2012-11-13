
// This needs to be in a library for test code. =(

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdlib.h>

#include <cppunit/TestCase.h>

#include <record.h>
#include <cluster.h>
#include <training.h>

typedef std::vector<std::string> Labels;

#include "testdata.h"
#include "testutils.h"

using std::string;
using std::vector;

std::vector<std::string>
setup_columns() {

  // Labels, as typedef'ed above. Keep the full type
  // definition here to help keep track of what things
  // are called in the main code.
  std::vector<std::string> involved_columns;
  involved_columns.push_back (std::string("Firstname"));
  involved_columns.push_back (std::string("Middlename"));
  involved_columns.push_back (std::string("Lastname"));
  involved_columns.push_back (std::string("Street"));
  involved_columns.push_back (std::string("City"));
  involved_columns.push_back (std::string("State"));
  involved_columns.push_back (std::string("Country"));
  involved_columns.push_back (std::string("Zipcode"));
  involved_columns.push_back (std::string("Latitude"));
  involved_columns.push_back (std::string("Longitude"));
  involved_columns.push_back (std::string("Patent"));
  involved_columns.push_back (std::string("ApplyYear"));
  involved_columns.push_back (std::string("Assignee"));
  involved_columns.push_back (std::string("AsgNum"));
  involved_columns.push_back (std::string("Class"));
  involved_columns.push_back (std::string("Coauthor"));
  involved_columns.push_back (std::string("Unique_Record_ID"));
  return involved_columns;
}


void
rare_names(std::list<Record> all_records) {

  RecordPList rare_firstname_set;
  RecordPList rare_lastname_set;
  std::vector<RecordPList *> rare_pointer_vec;
  rare_pointer_vec.push_back(&rare_firstname_set);
  rare_pointer_vec.push_back(&rare_lastname_set);
  list<const Record *> record_pointers;

  list<Record>::const_iterator riter = all_records.begin();
  for (; riter != all_records.end(); ++riter) {
    record_pointers.push_back(&(*riter));
  }

  find_rare_names_v2(rare_pointer_vec, record_pointers);
  std::cout << "End of rare_names in test" << std::endl;
}


class EngineConfigTest : public CppUnit::TestCase {

private:
  static const std::string LINE;
  vector<string> tcn;
  vector<string> requested_columns;
  vector<unsigned int> indices;

public:
  EngineConfigTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

 ~EngineConfigTest() {}

  void runTest() {

  std::vector<std::string> involved_columns = setup_columns();
  //printer(involved_columns);
  char filename[] = "./fixtures/csv/rjones.csv";
  std::list <Record> all_records;
  fetch_records_from_txt(all_records, filename, involved_columns);
  rare_names(all_records);
  }
};


void
test_engineconfig() {

  EngineConfigTest * ect = new EngineConfigTest("Engine configuration unit testing");
  ect->runTest();
  delete ect;
}


#ifdef test_engineconfig_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_engineconfig();
  return 0;
}
#endif
