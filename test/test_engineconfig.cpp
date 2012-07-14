
// This needs to be in a library for test code. =(

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdlib.h>

#include "record.h"

typedef std::vector<std::string> Labels;

void
printer(const Labels & l) {

  Labels::const_iterator iter = l.begin();
  for (; iter != l.end(); ++iter) {
    std::cout << (*iter).c_str() << std::endl;
  }
}

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



int
main(int argc, char ** argv) {

  std::vector<std::string> involved_columns = setup_columns();
  printer(involved_columns);
  char filename[] = "./testdata/invpat_onerecord.txt";
  std::list <Record> all_records;
  fetch_records_from_txt(all_records, filename, involved_columns);

  return 0;
}
