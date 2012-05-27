
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>

#include "txt2sqlite3.h"

using std::string;
using std::map;

// TODO: Get rid of this later
const unsigned int base = 100000;

bool
build_pragmas(sqlite3 * pDB) {

    const string commands [] = {
        "PRAGMA synchronous = OFF;",
        "PRAGMA cache_size = 8000; ",
        "PRAGMA temp_store = MEMORY; ",
        "PRAGMA journal_mode = MEMORY; ",
        "PRAGMA page_size = 8192; "
    };

    for ( unsigned int i = 0; i < sizeof(commands)/sizeof(string); ++i ) {
        int sqlres = sqlite3_exec(pDB, commands[i].c_str(), NULL, NULL, NULL);
        if ( SQLITE_OK != sqlres )
            std::cout << "Failed: ";
        else
            std::cout << "Success: ";
        std::cout << commands[i] << std::endl;
    }
}


bool
read_results(const char * txt_source,
             map < string, string > & update_dict) {

    std::ifstream instream(txt_source);
    static const char * primary_delim = "###";
    static const char * secondary_delim = ",";
    const unsigned int primary_delim_size = strlen(primary_delim);
    const unsigned int secondary_delim_size = strlen(secondary_delim);

    if (instream.good()) {

        string line;
        register size_t pos, prev_pos;

        while ( getline(instream, line)) {
            pos = prev_pos = 0;
            pos = line.find(primary_delim, prev_pos);
            string valuestring = line.substr( prev_pos, pos - prev_pos);
            prev_pos = pos + primary_delim_size;

            pos = line.find(primary_delim, prev_pos);
            prev_pos = pos + primary_delim_size;

            while ( ( pos = line.find(secondary_delim, prev_pos) )!= string::npos){
                map < string, string >::iterator pm;
                string keystring = line.substr( prev_pos, pos - prev_pos);
                pm = update_dict.find(keystring);
                if ( pm != update_dict.end() ) {
                    std::cout << "Duplicate records: " << keystring << std::endl;
                    return false;
                }
                update_dict.insert(std::pair<string,string>(keystring, valuestring));
                prev_pos = pos + secondary_delim_size;
            }
        }

    } else {
	// This is crazy! The message reported back needs to be extracted
	// from perror (or whatever stl uses), and not "File not found."
	// .good() is a member of iostream, it's not file method.
	// See for more information http://www.cplusplus.com/reference/iostream/ios/good/
        std::cout << "File not found: " << txt_source << std::endl;
        return false;
    }
}


bool
stepwise_add_column (const char * sqlite3_target,
                     const char * tablename,
                     const char * txt_source,
                     const string & unique_record_name,
                     const string & unique_inventor_name) {

    sqlite3 * pDB;
    int sqlres;
    std::cout << "Dumping " << txt_source << " to file << "
              << sqlite3_target << " >>, tablename << " << tablename
              << " >> ......" << std::endl;


    sqlres = sqlite3_open_v2(sqlite3_target, &pDB,
             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);


    // See http://www.cplusplus.com/reference/iostream/ios/good/
    // and implement a check_result() function.
    if (SQLITE_OK != sqlres ) {
        std::cout << "SQL DB open error." <<sqlres<< std::endl;
        return false;
    }

    // update_dict is our main data structure for handling
    // the results. We'll fill it with values from the text
    // file created by the disambiguation, then write those
    // values into an sqlite database for convenience.
    map < string, string > update_dict;
    // sync_with_stdio is static; we'll set it here for symmetry
    // with the reset a few lines below.
    std::ifstream::sync_with_stdio(false);
    bool result_val = read_results(txt_source, update_dict);
    if (result_val == false) return false;

    // TODO: Get rid of the sync, use one or the other, it's simpler.
    std::ifstream::sync_with_stdio(true);


    build_pragmas(pDB);    
    
    const unsigned int buff_size = 512;
    char buffer[buff_size];
    sqlite3_stmt *statement;


    sprintf( buffer, "CREATE TABLE %s ( %s) ;", tablename, unique_record_name.c_str());
    sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);


    if ( SQLITE_OK != sqlres ) {
        //std::cout  << tablename << " already exists."<< std::endl;
        //return 2;
    } else {

	//// TODO: Factor this out into its own function
        std::cout << tablename << " is created." << std::endl;
        sprintf ( buffer, "INSERT INTO %s VALUES (@KEY);", tablename );
        sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);

        if ( sqlres != SQLITE_OK ) {
            std::cout << "Statement preparation error: " << buffer << std::endl;
            std::cout << "Maybe the table name is invalid." << std::endl;
            return false;
        }

        unsigned int count = 0;
        sqlite3_exec(pDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        for (map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
            sqlite3_bind_text(statement, 1, cpm->first.c_str(), -1, SQLITE_TRANSIENT);

            sqlres = sqlite3_step(statement);
            if ( sqlres != SQLITE_DONE ) {
                std::cout << "Statement step error." << std::endl;
                return false;
            }
            sqlite3_clear_bindings(statement);
            sqlite3_reset(statement);
            ++count;
            if ( count % base == 0 )
                std::cout << count << " records has been initialized. " << std::endl;
        }
        sqlite3_exec(pDB, "END TRANSACTION;", NULL, NULL, NULL);
        std::cout << tablename << " is initialized." << std::endl;
	//// End refactor
    }


    /// Refactor into index creation function
    sprintf(buffer, "CREATE UNIQUE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;",
            unique_record_name.c_str(), tablename, tablename, unique_record_name.c_str() );
    sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);

    if ( SQLITE_OK != sqlres ) {
        std::cout << "Column " << unique_record_name << " does not exist. Adding column ..........   ";
        sprintf(buffer, "ALTER TABLE %s ADD COLUMN %s ;", tablename, unique_record_name.c_str() );
        sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);

        if ( SQLITE_OK != sqlres ) {
            std::cout <<  std::endl << " Error in adding column " << unique_record_name << std::endl;
            return 2;
        }

        std::cout << "Done." << std::endl;
        sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;",
                unique_record_name.c_str(), tablename, tablename, unique_record_name.c_str() );
        sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
        if ( SQLITE_OK != sqlres ) {
            std::cout <<  " Error in adding index " << unique_record_name << std::endl;
            return 3;
        }
    }
    /// End refactor


    // Probably refactor this one as well
    sprintf(buffer, "SELECT %s from %s; ", unique_inventor_name.c_str(), tablename);
    //sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;",
    //        unique_inventor_name.c_str(), tablename, tablename, unique_inventor_name.c_str() );
    sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);

    // This is essentially exception logic, very brittle.
    // TODO: Rewrite the logic to test first.
    if ( SQLITE_OK != sqlres ) {
        std::cout << "Column " << unique_inventor_name << " does not exist. Adding column ..........   ";
        sprintf(buffer, "ALTER TABLE %s ADD COLUMN %s ;", tablename, unique_inventor_name.c_str() );
        sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);

        if ( SQLITE_OK != sqlres ) {
            std::cout << std::endl << " Error in adding column " << unique_inventor_name << std::endl;
            return 2;
        }
        std::cout << "Done." << std::endl;
        /*
        sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;",
                unique_inventor_name.c_str(), tablename, tablename, unique_inventor_name.c_str() );
        sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
        if ( SQLITE_OK != sqlres ) {
            std::cout <<  " Error in adding index " << unique_inventor_name << std::endl;
            return 3;
        }
        */
    }
    // End of refactor


    sprintf(buffer, "UPDATE %s set %s = @VAL WHERE %s = @KEY;",
            tablename, unique_inventor_name.c_str(), unique_record_name.c_str());
    sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);

    // All of these need to be moved to a check_result function,
    // and errors handled with the sqlite error codes.
    if ( sqlres != SQLITE_OK ) {
        std::cout << "Statement preparation error: " << buffer << std::endl;
        std::cout << "Maybe the table name is invalid." << std::endl;
        return false;
    }

    sqlite3_exec(pDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    unsigned int count = 0;
    for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {

        sqlite3_bind_text(statement, 2, cpm->first.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 1, cpm->second.c_str(), -1, SQLITE_TRANSIENT);

        sqlres = sqlite3_step(statement);
        if ( sqlres != SQLITE_DONE ) {
            std::cout << "Statement step error." << std::endl;
            return false;
        }
        sqlite3_clear_bindings(statement);
        sqlite3_reset(statement);
        ++count;
        if ( count % base == 0 )
            std::cout << count << " records has been updated. " << std::endl;
    }
    sqlite3_exec(pDB, "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(statement);
    sqlite3_close(pDB);

    std::cout << "Dumping complete. " << std::endl;
    return true;
}


void
usage(const int argc) {

  if (argc != 6) {

    std::cout << "Invalid number of parameters. Should be 5 parameters." << std::endl;
    std::cout << "Usage: ./txt2sqlite3 target.sqlite3 tablename source.txt "
              << "UNIQUE_RECORD_COLUMN_NAME UNIQUE_INVENTOR_COLUMN_NAME" << std::endl
              << " ----------> dump the source.txt into target.sqlite3.table with "
              << "unique_record_id = UNIQUE_RECORD_COLUMN_NAME" << std::endl
              <<    "  and unique_inventor_id = UNIQUE_INVENTOR_COLUMN_NAME." << std::endl;
    exit(1);
  }
}


int
main( const int argc, const char * argv[] ) {

    const char * targetsqlite3 = argv[1];
    const char * tablename = argv[2];
    const char * sourcetxt = argv[3];
    const string unique_record_id ( argv[4]);
    const string unique_inventor_id( argv[5]);

    usage(argc);

    stepwise_add_column(targetsqlite3, tablename , sourcetxt, unique_record_id, unique_inventor_id);

    return 0;
}
