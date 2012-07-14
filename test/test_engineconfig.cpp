
// This needs to be in a library for test code. =(
//
//
//

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdlib.h>

#include "record.h"

void
printer(const std::vector<std::string> & columns) {

  std::vector<std::string>::const_iterator citer = columns.begin();
  for (; citer != columns.end(); ++citer) {
    std::cout << (*citer).c_str() << std::endl;
  }
}

std::vector<std::string>
setup_columns() {

  std::vector<std::string> involved_columns;
  involved_columns.push_back (std::string("Lastname"));
  involved_columns.push_back (std::string("Firstname"));
  involved_columns.push_back (std::string("Middlename"));
  printer(involved_columns);
  return involved_columns;
}



int
main(int argc, char ** argv) {

  std::vector<std::string> involved_columns = setup_columns();
  char filename[] = "./testdata/invpat_onerecord.txt";
  std::list <Record> all_records;
  fetch_records_from_txt(all_records, filename, involved_columns);

  return 0;
}
