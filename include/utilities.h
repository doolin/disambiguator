
#ifndef PATENT_UTILITIES_H
#define PATENT_UTILITIES_H

#include <vector>
#include <list>
#include <string>
#include <map>
using std::vector;
using std::list;
using std::string;
using std::map;

class Record;
class StringManipulator;
class ClusterSet;
class cRatios;


bool   make_changable_training_sets_by_patent   (const list <const Record*> & record_pointers,
                                                 const vector<string >& blocking_column_names,
                                                 const vector < const StringManipulator *> & pstring_oper,
                                                 const unsigned int limit,
                                                 const vector <string> & training_filenames);

bool   make_stable_training_sets_by_personal    (const list <Record> & all_records,
                                                 const unsigned int limit,
                                                 const vector <string> & training_filenames);

bool   make_changable_training_sets_by_assignee (const list <const Record*> & record_pointers,
                                                 const vector<string >& blocking_column_names,
                                                 const vector < const StringManipulator *> & pstring_oper,
                                                 const unsigned int limit,
                                                 const vector <string> & training_filenames);

int    unique_inventors_per_period              (unsigned int starting_year,
                                                 unsigned int interval,
                                                 const char * wholedatabase,
                                                 const char * disambigresult,
                                                 const char * outputfile);

/**
 * @param all_records are all the records, excellent.
 * @param last_disambig_result match file (?) from the last round.
 * @param outputfile currently hard coded as "final.txt" in disambiguate.cpp:650
 */
void   one_step_postprocess                     (const list < Record > & all_records,
                                                 const char * last_disambig_result,
                                                 const char * outputfile);

string remove_headtail_space                    (const string & s);

void   out_of_cluster_density                   (const ClusterSet & upper,
                                                 const ClusterSet & lower,
                                                 const cRatios & ratio,
                                                 std::ofstream & ofile );


#endif /* PATENT_UTILITIES_H */
