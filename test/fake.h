#ifndef PATENT_FAKE_H
#define PATENT_FAKE_H

#include <string>
#include <list>
#include <map>

#include <engine.h>
#include <record.h>
#include <ratios.h>
#include <blocking_operation.h>

#include <cppunit/TestCase.h>

class FakeTest : public CppUnit::TestCase {

private:

  list<Record> source;
  vector<string> requested_columns;
  RecordPList record_pointers;
  // Accessory container for unit testing, not
  // present in the disambiguation code.
  vector<const Record *> rpv;
  string csvfilename;
  map<string, const Record *> uid_dict;
  const cBlocking_Operation_By_Coauthors * coauthor_blocking;

public:

  FakeTest(string name, string filename);

  void load_fake_data(string csvfilename);

  RecordIndex * get_uid_dict();

  RecordPList get_recpointers();

  vector<const Record *> get_recvecs();

  const cBlocking_Operation_By_Coauthors * get_coauthor_blocking();

  void runTest() {
    load_fake_data(csvfilename);
  }
};


#endif //PATENT_FAKE_H
