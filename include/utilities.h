
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
class cCluster_Set;
class cRatios;


bool make_changable_training_sets_by_patent(const list <const Record*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const StringManipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
bool make_stable_training_sets_by_personal ( const list <Record> & all_records, const unsigned int limit, const vector <string> & training_filenames);
bool make_changable_training_sets_by_assignee(const list <const Record*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const StringManipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
int unique_inventors_per_period ( unsigned int starting_year, unsigned int interval, const char * wholedatabase, const char * disambigresult, const char * outputfile);
void one_step_prostprocess( const list < Record > & all_records, const char * last_disambig_result, const char * outputfile);
string remove_headtail_space( const string & s );
void out_of_cluster_density( const cCluster_Set & upper, const cCluster_Set & lower, const cRatios & ratio, std::ofstream & ofile );


#endif /* PATENT_UTILITIES_H */
