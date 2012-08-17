
// This needs to be in a library for test code. =(

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdlib.h>

#include "record.h"
#include "cluster.h"
#include "training.h"

typedef std::vector<std::string> Labels;

/*
void
printer(const Labels & l) {

  Labels::const_iterator iter = l.begin();
  for (; iter != l.end(); ++iter) {
    std::cout << (*iter).c_str() << std::endl;
  }
}
*/

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

  cGroup_Value rare_firstname_set;
  cGroup_Value rare_lastname_set;
  std::vector<cGroup_Value *> rare_pointer_vec;
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

int
main(int UP(argc), char ** UP(argv)) {

  std::vector<std::string> involved_columns = setup_columns();
  //printer(involved_columns);
  //char filename[] = "./testdata/invpat2.txt";
  char filename[] = "/var/share/patentdata/patents/2010/invpat.csv";
  std::list <Record> all_records;
  fetch_records_from_txt(all_records, filename, involved_columns);
  rare_names(all_records);
 
  return 0;
}
