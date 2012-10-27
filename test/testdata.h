
#ifndef PATENT_TEST_TESTDATA_H
#define PATENT_TEST_TESTDATA_H

#include <engine.h> // for RecordList
#include <record.h>
#include <blocking_operation.h>

Record     * make_foobar_record(void);

Record     * make_quuxalot_record(void);

RecordPList   get_record_list(void);

cBlocking_Operation_By_Coauthors get_blocker_coathor(void);

vector<std::string> get_column_names(void);

#endif // PATENT_TEST_TESTDATA_H
