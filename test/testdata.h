
#ifndef PATENT_TEST_TESTDATA_H
#define PATENT_TEST_TESTDATA_H

#include <engine.h> // for RecordList
#include <record.h>
#include <blocking_operation.h>

Record     * make_foobar_record(void);

Record     * make_quuxalot_record(void);

RecordList   get_record_list(void);

cBlocking_Operation_By_Coauthors get_blocker_coathor(void);

#endif // PATENT_TEST_TESTDATA_H
