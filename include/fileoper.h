#ifndef PATENT_FILEOPER_H
#define PATENT_FILEOPER_H

#include <iostream>
#include <map>
#include <set>
#include <list>


using std::string;
using std::set;
using std::map;
using std::list;

class Record;

const Record * retrieve_record_pointer_by_unique_id(const string & uid,
                                                    const map<string, const Record*> & uid_tree);

void           create_btree_uid2record_pointer     (map<string, const Record *> & uid_tree,
                                                    const list<Record> & reclist,
                                                    const string & uid_name );

bool           dump_match                          (const char * sqlite3_target,
                                                    const char * tablename,
                                                    const char * txt_source,
                                                    const string & unique_record_name,
                                                    const string & unique_inventor_name);

#endif /* PATENT_FILEOPER_H */
