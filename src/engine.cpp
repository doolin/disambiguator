
#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <numeric>

#include "attribute.h"
#include "cluster.h"
#include "ratios.h"
#include "newcluster.h"

using std::map;
using std::set;



/*
 * Aim: to find a ratio that corresponds to a given similarity
 * profile in a given similarity profile binary tree.
 * Algorithm: STL map find.
 */
double
fetch_ratio(const vector < uint32_t > & ratio_to_lookup,
            const map <SimilarityProfile, double, SimilarityCompare > & ratiosmap ) {
            //const map <vector <uint32_t>, double, SimilarityCompare > & ratiosmap ) {

    //map < vector <uint32_t>, double, SimilarityCompare >::const_iterator p = ratiosmap.find( ratio_to_lookup);
    map <SimilarityProfile, double, SimilarityCompare >::const_iterator p = ratiosmap.find( ratio_to_lookup);
    if ( p == ratiosmap.end())
        return 0;
    else
        return p->second;
}


std::pair<const Record *, double>
disambiguate_by_set (const Record * key1,
                     const RecordPList & match1,
                     const double cohesion1,
                     const Record * key2,
                     const RecordPList & match2,
                     const double cohesion2,
                     const double prior,
                     const cRatios & ratio,  const double mutual_threshold ) {

    static const uint32_t firstname_index = Record::get_similarity_index_by_name(cFirstname::static_get_class_name());
    static const uint32_t midname_index = Record::get_similarity_index_by_name(cMiddlename::static_get_class_name());
    static const uint32_t lastname_index = Record::get_similarity_index_by_name(cLastname::static_get_class_name());
    static const uint32_t country_index = Record::get_index_by_name(cCountry::static_get_class_name());
    static const bool country_check = true;


    //prescreening.
    const bool prescreening = true;
    if ( prescreening ) {
        if ( country_check ) {
            const Attribute * p1 = key1->get_attrib_pointer_by_index(country_index);
            const Attribute * p2 = key2->get_attrib_pointer_by_index(country_index);
            if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
                return std::pair<const Record *, double> (NULL, 0);
        }


        vector < uint32_t > screen_sp = key1->record_compare(*key2);
        const double screen_r = fetch_ratio(screen_sp, ratio.get_ratios_map());
        const double screen_p = 1.0 / ( 1.0 + ( 1.0 - prior )/ prior / screen_r );
        if ( screen_p < 0.3 || screen_sp.at(firstname_index) == 0 || screen_sp.at(midname_index) == 0 || screen_sp.at(lastname_index) == 0 )
            return std::pair<const Record *, double> (NULL, 0);
    }
    const bool partial_match_mode = true;

    const double minimum_threshold = 0.7;
    const double threshold = max_val <double> (minimum_threshold, mutual_threshold * cohesion1 * cohesion2);
    static const cException_Unknown_Similarity_Profile except(" Fatal Error in Disambig by set.");

    const uint32_t match1_size = match1.size();
    const uint32_t match2_size = match2.size();

    //const uint32_t required_candidates = static_cast< uint32_t > ( 1.0 * sqrt(1.0 * match1_size * match2_size ));
    //const uint32_t candidates_for_averaging = 2 * required_candidates - 1 ;
    uint32_t candidates_for_averaging = match1_size * match2_size / 4 ;
    if ( candidates_for_averaging == 0 )
        candidates_for_averaging = 1;
    if ( candidates_for_averaging == 0 )
        throw cException_Other("Computation of size of averaged probability is incorrect.");

    set < double > probs;

    double interactive = 0;
    double cumulative_interactive = 0;
    uint32_t qualified_count = 0;
    //double required_interactives = 0;
    //uint32_t required_cnt = 0;
    for ( RecordPList::const_iterator p = match1.begin(); p != match1.end(); ++p ) {
        for ( RecordPList::const_iterator q = match2.begin(); q != match2.end(); ++q ) {

            if ( country_check ) {
                const Attribute * p1 = (*p)->get_attrib_pointer_by_index(country_index);
                const Attribute * p2 = (*q)->get_attrib_pointer_by_index(country_index);
                if ( p1 != p2 && p1->is_informative() && p2->is_informative() )
                    return std::pair<const Record *, double> (NULL, 0);
            }



            vector< uint32_t > tempsp = (*p)->record_compare(* *q);
            if ( tempsp.at(firstname_index) == 0 || tempsp.at(midname_index) == 0 || tempsp.at(lastname_index) == 0 )
                return std::pair<const Record *, double> (NULL, 0);


            double r_value = fetch_ratio(tempsp, ratio.get_ratios_map());

            if ( r_value == 0 ) {
                interactive += 0;
            }
            else {
                const double temp_prob = 1.0 / ( 1.0 + ( 1.0 - prior )/prior / r_value );
                interactive +=  temp_prob ;
                if ( partial_match_mode && qualified_count < candidates_for_averaging ) {
                    if (  probs.size() >= candidates_for_averaging ) {
                        probs.erase(probs.begin());
                    }
                    probs.insert(temp_prob);
                }
                if ( partial_match_mode && temp_prob >= threshold ) {
                    cumulative_interactive += temp_prob;
                    ++qualified_count;
                }
            }
        }
    }

    const double interactive_average = interactive / match1_size / match2_size;
    double probs_average;

    if ( qualified_count > probs.size() )
        probs_average = cumulative_interactive / qualified_count;
    else
        probs_average = std::accumulate(probs.begin(), probs.end(), 0.0 ) / probs.size();

    if ( interactive_average > 1 )
        throw cException_Invalid_Probability("Cohesion value error.");

    if ( partial_match_mode && probs_average < threshold )
        return std::pair<const Record *, double> (NULL, probs_average);
    if ( ( ! partial_match_mode ) && interactive_average < threshold )
        return std::pair<const Record *, double> (NULL, interactive_average);


    double inter = 0;
    if ( partial_match_mode )
        inter = probs_average * match1_size * match2_size;
    else
        inter = interactive;

    const double probability = ( cohesion1 * match1_size * ( match1_size - 1 )
                                + cohesion2 * match2_size * ( match2_size - 1 )
                                + 2.0 * inter ) / ( match1_size + match2_size) / (match1_size + match2_size - 1 );
    //ATTENSION: RETURN A NON-NULL POINTER TO TELL IT IS A MERGE. NEED TO FIND A REPRESENTATIVE IN THE MERGE PART.
    return std::pair<const Record *, double>( key1, probability );

}


void
copyfile(const char * target, const char * source) {

    std::cout << "Copying file " << source << " to " << target << std::endl;
    std::ifstream   input( source,std::ios::binary);
    std::ofstream   output( target,std::ios::binary);

    output   <<   input.rdbuf();

    std::cout << "File copy done." << std::endl;
}


std::vector<std::string>
parse_column_names(std::string line) {

  int pos = 0;
  int prev_pos = 0;
  const char * delim = ",";
  const uint32_t delim_size = strlen(delim);
  vector <string> total_col_names;

  while (pos != string::npos) {

      string columnname;

      pos = line.find(delim, prev_pos);
      if (pos != string::npos) {
          columnname = line.substr(prev_pos, pos - prev_pos);
      } else {
          columnname = line.substr(prev_pos );
      }

      total_col_names.push_back(columnname);
      prev_pos = pos + delim_size;
  }
  return total_col_names;
}



// TODO: Unit test this thing.
vector<uint32_t>
create_column_indices(std::vector<std::string> requested_columns,
    std::vector<std::string> total_col_names) {

  std::vector<uint32_t> rci;
  const uint32_t num_cols = requested_columns.size();

  for (uint32_t i = 0; i < num_cols; ++i) {

      uint32_t j;
      for (j = 0; j < total_col_names.size(); ++j) {
          if (requested_columns.at(i) == total_col_names.at(j)) {
              rci.push_back(j);
              break;
          }
      }

      if (j == total_col_names.size()) {
          std::cerr << "Critical Error in reading data input file" << std::endl
          <<"Column names not available in the first line. Please Check the correctness." << std::endl;
          throw cException_ColumnName_Not_Found(requested_columns.at(i).c_str());
      }
  }
  return rci;
}


Attribute **
instantiate_attributes(std::vector<std::string> column_names, int num_cols) {

  Attribute ** pointer_array = new Attribute *[num_cols];

  uint32_t pos =  0;
  uint32_t prev_pos = 0;
  uint32_t position_in_ratios = 0;

  for (uint32_t i = 0; i < num_cols; ++i) {

      //std::cout << "column_names[i]: " << column_names[i] << std::endl;
      const int pos_in_query = Attribute::position_in_registry(column_names[i]);
      //std::cout << "pos_in_query: " << pos_in_query << std::endl;
      //std::cout << "column_names[i]: " << column_names[i] << std::endl;

      if ( pos_in_query == -1 ) {
          for ( uint32_t j = 0; j < i; ++j )
              delete pointer_array[j];
          delete [] pointer_array;
          throw cException_ColumnName_Not_Found(column_names[i].c_str());
      } else {
          pointer_array[i] = create_attribute_instance (column_names[i].c_str() );
      }

#ifdef HLKJHLKJJHGKJHKJHKFHGFKHG
      if ( Record::column_names[i] == cLongitude::class_name ) {
          cLatitude::interactive_column_indice_in_query.push_back(i);
      }
      else if ( Record::column_names[i] == cStreet::class_name ) {
          cLatitude::interactive_column_indice_in_query.push_back(i);
      }
      else if ( Record::column_names[i] == cCountry::class_name ) {
          cLatitude::interactive_column_indice_in_query.push_back(i);
      }
      else if ( Record::column_names[i] == cAsgNum::class_name ) {
          cAssignee::interactive_column_indice_in_query.push_back(i);
      }
#endif

      // If this crashes, will need to add code in the function `create_attribute_instance`
      // Also, it's stupid that this is used to check whether something is
      // instantiated.
      if ( pointer_array[i]->get_attrib_group() != string("None") ) {
          //std::cout << "pointer_array[i]->get_attrib_group(): " << pointer_array[i]->get_attrib_group() << std::endl;
          // TODO: What is this for? Get rid of it.
          ++position_in_ratios;
      }
  }
  return pointer_array;
}


vector <const Attribute *>
parse_line(string line,
           vector<uint32_t> requested_column_indice,
           Attribute ** pointer_array,
           uint32_t num_cols,
           const char * delim,
           vector<string> & string_cache) {

  const uint32_t delim_size = strlen(delim);
  vector <const Attribute *> temp_vec_attrib;
  const Attribute * pAttrib;

  for (uint32_t i = 0; i < num_cols ; ++i) {

      // What does column_location point at?
      uint32_t column_location = 0;
      // Given the second iteration, what does prev_pos point at?
      uint32_t pos = 0;
      uint32_t prev_pos = 0;

      while (column_location++ != requested_column_indice.at(i)) {
          pos = line.find(delim, prev_pos);
          prev_pos = pos + delim_size;
      }

      pos = line.find(delim, prev_pos);

      // Find a link to string::npos
      if (pos == string::npos) {

          // Find a link to .size for whatever type line is
          if ( prev_pos != line.size() ) {
              // Link to the substr method
              // I don't understand why the work "link" keeps
              // getting used for variable assignment.
              // TODO: Write a blog post about this, and about the
              // general notion of using generally accepted labels
              // for common things instead fucking making up shit
              // arbitrarily. It's variable assignment, not "linking".
              string_cache[i] = line.substr(prev_pos);
          } else {
              string_cache[i] = "";
          }

      } else {
          // This looks where the actual value is parsed
          string_cache[i] = line.substr(prev_pos, pos - prev_pos);
      }

      // Link to the reset_data method
      // Why is this here? What purpose is it serving? This smells.
      // TODO: Figure out why this is here and see about putting
      // it somewhere else. Definitely unit test this 
      pointer_array[i]->reset_data(string_cache[i].c_str());
      // Link to the clone method
      //HERE CREATED NEW CLASS INSTANCES.
      pAttrib = pointer_array[i]->clone();
      temp_vec_attrib.push_back(pAttrib);
  }

  return temp_vec_attrib;
}


void
print_involved_attributes(Attribute ** pointer_array,
    uint32_t num_cols) {

    std::cout << "Involved attributes are: ";
    for (uint32_t i = 0; i < num_cols; ++i) {
        std::cout << pointer_array[i]->get_class_name() << ", ";
    }
    std::cout << std::endl;
}


void
print_polymorphic_data_types(Attribute ** pointer_array,
    uint32_t num_cols) {

    std::cout << "Polymorphic data types are: ";
    for (uint32_t i = 0; i < num_cols; ++i) {
        std::cout << typeid(*pointer_array[i]).name()<< ", ";
    }
    std::cout << std::endl;
}


void
check_interactive_consistency(Attribute ** pointer_array,
    uint32_t num_cols) {

}


/**
 * First of all, read the first line in the file. The first
 * line should include all the information of each column. ie. They
 * are usually the * column names. The format of the first line is
 * "Column Name1,Column Name 2,Column Name3,...,Column Name Last".If the delimiter is not
 * comma, change the function variable "delim".
 * Second, check the argument "requested_columns" in all the columns,
 * and record the indice of requested_columns
 * Third, starting from the second line to the end of the file,
 * read relevant information with the help of delimiters and indice,
 * and save them in appropriate attributes.
 * Finally, do some concrete class related stuff, like setting
 * static members and run reconfigurations.
 */
bool
fetch_records_from_txt(list <Record> & source,
                       const char * txt_file,
                       const vector<string> & requested_columns) {

    std::ifstream::sync_with_stdio(false);
    // the "," deliminator should never occur in the data.
    const char * delim = ",";
    const uint32_t delim_size = strlen(delim);
    register size_t pos, prev_pos;

    std::ifstream instream(txt_file);
    if (!instream.good()) {
        throw cException_File_Not_Found(txt_file);
    }

    string line;
    getline(instream, line);
    vector<string> total_col_names = parse_column_names(line);

    Attribute::register_class_names(requested_columns);
    vector<uint32_t> requested_column_indice;
    const uint32_t num_cols = requested_columns.size();
    requested_column_indice = create_column_indices(requested_columns, total_col_names);

    Record::column_names = requested_columns;
    Attribute ** pointer_array; 
    pointer_array = instantiate_attributes(Record::column_names, num_cols);


    // TODO: See if this can be moved to the instantiate_attributes function.
    // Or to it's own function which is called from instantiate attributes
    // always do this for all the attribute classes
    for (uint32_t i = 0; i < num_cols; ++i) {
        pointer_array[i]->check_interactive_consistency(Record::column_names);
    }

    // TODO: Move to its own function, get it covered
    // with a unit test, call it from somewhere else.
    std::cout << "Polymorphic data types are: ";
    vector <string> string_cache(num_cols);
    const uint32_t string_cache_size = 2048;
    for ( uint32_t i = 0; i < num_cols; ++i ) {
        string_cache.at(i).reserve(string_cache_size);
    }

    //std::cout << "Polymorphic data types are: ";
    std::cout << "Reading " << txt_file << " ......"<< std::endl;

    unsigned long size = 0;
    const uint32_t base  =  100000;
    const Attribute * pAttrib;
    vector <const Attribute *> temp_vec_attrib;
    //vector <const Attribute *> Latitude_interactive_attribute_pointers;

    // Extracts characters from ifstream (instream) and stores them 
    // into a string (line) until a delimitation character is found.
    // In this case, since a delimiter isn't given, it's assumed to be \n.
    std::cout << "Reading input data file..." << std::endl;
    // TODO: Move all this to its own function
    while (getline(instream, line)) {

        //std::cout << "line: " << line << std::endl;

        // vector.clear is an stl method, calls all destructors
        temp_vec_attrib.clear();

        // num_cols is obtained around Line 736 above
        // TODO: Move all this to its own function as well,
        // such that after some setup, it can be fed a line of
        // data and get itself parsed correctly. The tricky
        // part here is getting the right object copied back.
#if 1
        for (uint32_t i = 0; i < num_cols ; ++i) {

            // What does column_location point at?
            uint32_t column_location = 0;
            // Given the second iteration, what does prev_pos point at?
            pos = prev_pos = 0;

            // requested_column_indice is vector<uint32_t> defined
            // around Line 738 above, see vector.at
            while (column_location++ != requested_column_indice.at(i)) {
                // See string.find: http://www.cplusplus.com/reference/string/string/find/
                pos = line.find(delim, prev_pos);
                // delim_size is defined above Line ~705
                prev_pos = pos + delim_size;
            }
            pos = line.find(delim, prev_pos);

            // Find a link to string::npos
            if ( pos == string::npos ) {
                // Find a link to .size for whatever type line is
                if ( prev_pos != line.size() )
                    // Link to the substr method
                    string_cache[i] = line.substr(prev_pos);
                else
                    string_cache[i] = "";
            } else {
                // This looks where the actual value is parsed
                string_cache[i] = line.substr(prev_pos, pos - prev_pos);
            }

            // Link to the reset_data method
            // Why is this here? What purpose is it serving? This smells.
            // TODO: Figure out why this is here and see about putting
            // it somewhere else.
            pointer_array[i]->reset_data(string_cache[i].c_str());
            // Link to the clone method
            pAttrib = pointer_array[i]->clone();    //HERE CREATED NEW CLASS INSTANCES.
            temp_vec_attrib.push_back(pAttrib);
        }
#else
        temp_vec_attrib = parse_line(string line,
                                     vector<uint32_t> requested_column_indice,
                                     Attribute ** pointer_array,
                                     uint32_t num_cols,
                                     const char * delim,
                                     vector<string> & string_cache);
#endif

        Record temprec(temp_vec_attrib);
        source.push_back( temprec );

        ++size;
        if (size % base == 0) {
            std::cout << size << " records obtained." << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << size << " records have been fetched from "<< txt_file << std::endl;

    // TODO: What is this thing?
    Record::sample_record_pointer = & source.front();

    // TODO: Create a little function for this which can be unit tested
    for (uint32_t i = 0; i < num_cols; ++i) {
        delete pointer_array[i];
    }
    delete [] pointer_array;

    // TODO: Put this in it's own function and unit test it.
    for (list< Record>::iterator ci = source.begin(); ci != source.end(); ++ci) {
        ci->reconfigure_record_for_interactives();
    }

    // TODO: Remove this code or put it somewhere else.
    std::cout << "Sample Record: " << __FILE__ << ":" << __LINE__ << "---------" << std::endl;
    Record::sample_record_pointer->print();
    std::cout << "-----------------" << std::endl;

    return true;
}


Attribute *
create_attribute_instance ( const string & id ) {

    Attribute *p = NULL;

    if ( id == cFirstname::static_get_class_name() ) {
        p = new cFirstname;
    }
    else if ( id == cLastname::static_get_class_name() ) {
        p = new cLastname;
    }
    else if ( id == cMiddlename::static_get_class_name() ) {
        p = new cMiddlename;
    }
    else if ( id == cLatitude::static_get_class_name() ) {
        p = new cLatitude;
    }
    else if ( id == cLongitude::static_get_class_name() ) {
        p = new cLongitude;
    }
    else if ( id == cStreet::static_get_class_name() ) {
        p = new cStreet;
    }

    else if ( id == cState::static_get_class_name() ) {
        p = new cState;
    }
    else if ( id == cZipcode::static_get_class_name() ) {
        p = new cZipcode;
    }
    else if ( id == cInvSeq::static_get_class_name() ) {
        p = new cInvSeq;
    }
    else if ( id == cinvnum_N::static_get_class_name() ) {
        p = new cinvnum_N;
    }
    else if ( id == cinvnum::static_get_class_name() ) {
        p = new cinvnum;
    }

    //else if ( id == cAppDateStr::static_get_class_name() ) {
    //   p = new cAppDateStr;
    // }
    else if ( id == cAppDate::static_get_class_name() ) {
        p = new cAppDate;
    }
    else if ( id == cCountry::static_get_class_name() ) {
        p = new cCountry;
    }
    else if ( id == cClass::static_get_class_name() ) {
        p = new cClass;
    }
    else if ( id == cCoauthor::static_get_class_name() ) {
        p = new cCoauthor;
    }
    else if ( id == cAssignee::static_get_class_name() ) {
        p = new cAssignee;
    }

    else if ( id == cAsgNum::static_get_class_name() ) {
        p = new cAsgNum;
    }
    else if ( id == cUnique_Record_ID::static_get_class_name() ) {
        p = new cUnique_Record_ID;
    }
    else if ( id == cApplyYear::static_get_class_name() ) {
        p = new cApplyYear;
    }
    else if ( id == cAppYear::static_get_class_name() ) {
        p = new cAppYear;
    }

    else if ( id == cGYear::static_get_class_name() ) {
        p = new cGYear;
    }
    else if ( id == cCity::static_get_class_name() ) {
        p = new cCity;
    }
    else if ( id == cPatent::static_get_class_name() ) {
        p = new cPatent;
    }
    else if ( id == cClass_M2::static_get_class_name() ) {
        p = new cClass_M2;
    }
    else {
        p = NULL;
    }

    return p;
}


void
reconfigure_interactives (const Record_Reconfigurator * pc,
                          const Record * pRec) {

    pc->reconfigure(pRec);
}


void
cAssignee::configure_assignee( const list < const Record *> & recs) {

    static const uint32_t asgnumidx = Record::get_index_by_name(cAsgNum::static_get_class_name());

    for ( list< const Record *>::const_iterator p = recs.begin(); p != recs.end(); ++p ) {
        const cAsgNum * pasgnum = dynamic_cast < const cAsgNum *> ( (*p)->get_attrib_pointer_by_index(asgnumidx) );
        if ( ! pasgnum ) {
            throw cException_Other("Cannot perform dynamic cast to cAsgNum.");
        }
        ++cAssignee::asgnum2count_tree[pasgnum];
    }

    cAssignee::is_ready = true;
}


void
build_patent_tree(PatentTree & patent_tree, const RecordPList & all_rec_pointers) {

    PatentTree::iterator ppatentmap;
    RecordPList::const_iterator p = all_rec_pointers.begin();
    for (; p != all_rec_pointers.end(); ++p) {
        ppatentmap = patent_tree.find(*p);
        if ( ppatentmap == patent_tree.end() ) {
            RecordPList temp ( 1, *p);
            patent_tree.insert( std::pair < const Record *, RecordPList > (*p, temp) );
        }
        else {
            ppatentmap->second.push_back(*p);
        }
    }
}


void
build_patent_tree(PatentTree & patent_tree,
                  const list<Record> & all_records) {

    RecordPList all_pointers;
    list<Record>::const_iterator p = all_records.begin();
    for (; p != all_records.end(); ++p) {
        all_pointers.push_back(&(*p));
    }
    build_patent_tree(patent_tree, all_pointers);
}


string
check_file_existence(const string & description) {

    std::ifstream infile;
    while ( true ) {
        string file;
        std::cout << "Enter file name for " << description << " : ";
        std::cin >> file;
        infile.open(file.c_str(), std::ios::in);
        if ( infile.good() ) {
            infile.close();
            std::cout << file << " accepted." << std::endl;
            return file;
        }
        else {
            std::cerr << file << " does not exist. Try again." << std::endl;
            infile.close();
        }
    }
}
