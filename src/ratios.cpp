
#include <cstring>

#include "ratios.h"
#include "engine.h"


const char * cRatios::primary_delim = "#";
const char * cRatios::secondary_delim = ",";
// TODO: Use #define LAPLACE_BASE 5 instead
const uint32_t cRatioComponent::laplace_base = 5;


vector<uint32_t>
get_max_similarity(const vector<string> & attrib_names)  {

  // TODO: SimilarityProfile sp;
    vector<uint32_t> sp;

    vector<string>::const_iterator p = attrib_names.begin();
    for (; p != attrib_names.end(); ++p) {
        const Attribute * pAttrib = Record::get_sample_record().
                                    get_attrib_pointer_by_index(Record::get_index_by_name(*p));
        const uint32_t max_entry = pAttrib->get_attrib_max_value();
        sp.push_back(max_entry);
    }
    return sp;
}


// TODO: make this utility function work
void
print_similarity_profile_size(const SimilarityProfile & sp) {

#if 1
    std::cout << "Size of Similarity Profile = "
              <<  sp.size()
              << ". Similarity Profile = ";

    vector<uint32_t>::const_iterator tt = sp.begin();
    for (; tt != sp.end(); ++tt) {
        std::cout << *tt << ":";
    }
    std::cout << std::endl;
#endif

}


cRatioComponent::cRatioComponent(const RecordIndex & uid_tree,
                                 const string & groupname)
            : attrib_group(groupname), puid_tree(&uid_tree), is_ready(false) {
};



/**
 * Count the occurrences of each distinct similarity profile.
 *
 * Loop over all the training pairs, compute the similarity
 * profile for each pair, increment the similarity profile count.
 *
 * TODO: Refactor the record checks.
 */
void
cRatioComponent::sp_stats (const TrainingPairs & trainpairs,
                           SPCountsIndex & sp_counts) const {

    const vector<uint32_t> & component_indice_in_record = get_component_positions_in_record();

    const RecordIndex & dict = *puid_tree;
    RecordIndex::const_iterator pm;
    SPCountsIndex::iterator sp_iter;

    TrainingPairs::const_iterator p = trainpairs.begin();
    for (; p != trainpairs.end(); ++p) {

        // TODO: Refactor these next two blocks to enforce DRY
        pm = dict.find(p->first);
        if (pm == dict.end()) {
            throw cException_Attribute_Not_In_Tree(
                (string("\"") + p->first + string ("\"") ).c_str());
        }
        const Record * plhs = pm->second;

        pm = dict.find(p->second);
        if (pm == dict.end()) {
            throw cException_Attribute_Not_In_Tree(
                (string("\"") + p->second + string ("\"") ).c_str());
        }
        const Record * prhs = pm->second;

        SimilarityProfile sp = plhs->record_compare_by_attrib_indice(*prhs, component_indice_in_record);
        //print_similarity_profile_size(sp);

        sp_iter = sp_counts.find(sp);

        if (sp_iter == sp_counts.end()) {
            sp_counts.insert(std::pair<SimilarityProfile, uint32_t>(sp, 1));
        } else {
            ++(sp_iter->second);
        }
    }
}


void
cRatioComponent::read_train_pairs(TrainingPairs & trainpairs,
                                  const char * txt_file) const {

    std::cout << "Reading training pairs from " << txt_file
              << ", " << __FILE__ << ":" << __LINE__ << std::endl;

    static const char * delim = ",";
    static const uint32_t delim_size = strlen(delim);

    std::ifstream::sync_with_stdio(false);
    std::ifstream infile(txt_file);

    if (infile.good()) {

        string filedata;

        while (getline(infile, filedata)) {
            register size_t pos = 0, prev_pos = 0;
            pos = filedata.find(delim, prev_pos);
            string firststring = filedata.substr( prev_pos, pos - prev_pos);
            prev_pos = pos + delim_size;
            pos = filedata.find(delim, prev_pos);
            string secondstring = filedata.substr(prev_pos, pos);
            trainpairs.push_back(TrainingPair(firststring, secondstring));
        }

        std::cout << txt_file << " has been loaded as the "
                  << attrib_group << " part of the training sets."<< std::endl;
    } else {
        throw cException_File_Not_Found(txt_file);
    }
}


void
cRatioComponent::stats_output(const char * filename) const {

    std::ofstream ostream (filename);

    const string nmc     = "Non-Match Counts";
    const string mc      = "Match Counts";
    const string splabel = "Similarity Profiles";
    const string delim   = " | ";

    ostream << splabel  << "(";
    vector<uint32_t>::const_iterator tt = this->positions_in_record.begin(); 
    for (tt; tt != this->positions_in_record.end(); ++tt) {
        ostream << Record::get_column_names().at(*tt) << ",";
    }
    ostream << ")";

    ostream << delim << mc << delim << nmc << '\n';
    map<vector<uint32_t>, uint32_t>::const_iterator pm;
    map<vector<uint32_t>, double>::const_iterator p;

    for (p = this->ratio_map.begin(); p != this->ratio_map.end(); ++p) {
        for (vector <uint32_t >::const_iterator q = p->first.begin(); q != p->first.end(); ++q ) {
            ostream << *q << ",";
        }

        ostream << delim;
        pm = this->m_counts.find(p->first);
        ostream << pm->second << delim;
        pm = this->x_counts.find(p->first);
        ostream << pm->second << '\n';
    }

    for (pm = this->m_counts.begin(); pm != this->m_counts.end(); ++pm ) {
        p = this->ratio_map.find(pm->first);

        if (p != this->ratio_map.end()) continue;

        for (vector<uint32_t>::const_iterator q = pm->first.begin(); q != pm->first.end(); ++q) {
            ostream << *q << ",";
        }

        ostream << delim;
        ostream << pm->second << delim;
        ostream << 0 << '\n';
    }

    for (pm = this->x_counts.begin(); pm != this->x_counts.end(); ++pm ) {

        p = this->ratio_map.find(pm->first);

        if (p != this->ratio_map.end()) {
            continue;
        }

        for (vector<uint32_t>::const_iterator q = pm->first.begin(); q != pm->first.end(); ++q) {
            ostream << *q << ",";
        }

        ostream << delim;
        ostream << 0 << delim;
        ostream << pm->second << '\n';
    }
    std::cout << "Ratio component info saved in " << filename << std::endl;
}


void
cRatioComponent::create_ratios() {

  SPCountsIndex::const_iterator p, q;

  // ratios = count of match / count of non-match;
  // We'll walk the non-match training set as its bigger
  // by definition. This procedure will miss similarities
  // occurring only in the match set; mitigating that has
  // to be done elsewhere.
  for (p = x_counts.begin(); p != x_counts.end(); ++p) {

      q = m_counts.find(p->first);
      if (q == m_counts.end()) {
          continue;
      } else {
          ratio_map.insert(std::pair<SimilarityProfile, double>
              (p->first, 1.0 * q->second / p->second));
      }
  }

}


void
cRatioComponent::prepare(const char * x_file,
                         const char * m_file) {

    //list<std::pair<string, string>> x_list, m_list;
    TrainingPairs x_list, m_list;
    x_counts.clear();
    m_counts.clear();
    ratio_map.clear();

    get_similarity_info();

    read_train_pairs(x_list, x_file);
    read_train_pairs(m_list, m_file);

    // sp_stats builds the count indexes for
    // match and non-match SimilarityProfiles.
    sp_stats(x_list, x_counts);
    sp_stats(m_list, m_counts);


    //////////////////////////////
    // TODO: Refactor into a laplace correction function.
    std::cout << "Before LAPLACE CORRECTION: " << std::endl;
    std::cout << "Size of non-match pair list = " << x_list.size() << std::endl;
    std::cout << "Size of match pair list = " << m_list.size() << std::endl;

    std::cout << "Non-match unique profile number = " << x_counts.size() << std::endl;
    std::cout << "Match unique profile number = " << m_counts.size() << std::endl;

    // laplace correction
    SPCountsIndex::const_iterator p, q;
    // TODO: document count_to_consider, move the value into a
    // #define in the appropriate header.
    const uint32_t count_to_consider = 100;
    //set <SimilarityProfile, SimilarityCompare > all_possible;
    set <vector<uint32_t>, SimilarityCompare > all_possible;

    for (p = x_counts.begin(); p != x_counts.end(); ++p) {

        if (m_counts.find(p->first) == m_counts.end() &&
            p->second < count_to_consider) {
            continue;
        } else {
            all_possible.insert(p->first);
        }
    }

    for (p = m_counts.begin(); p != m_counts.end(); ++p) {

        if (x_counts.find(p->first) == x_counts.end() &&
            p->second < count_to_consider) {
            continue;
        } else {
            all_possible.insert(p->first);
        }
    }

    //set<SimilarityProfile, SimilarityCompare >::const_iterator ps = all_possible.begin();
    set<vector<uint32_t>, SimilarityCompare >::const_iterator ps = all_possible.begin();
    for (; ps != all_possible.end(); ++ps) {

        SPCountsIndex::iterator p = x_counts.find(*ps);

        if (x_counts.end() == p) {
            x_counts.insert(std::pair<vector<uint32_t>, uint32_t>(*ps, laplace_base));
        } else {
            p->second += laplace_base;
        }

        p = m_counts.find(*ps);
        if (m_counts.end() == p) {
            m_counts.insert(std::pair<vector<uint32_t>, uint32_t>(*ps, laplace_base));
        } else {
            p->second += laplace_base;
        }
    }

    std::cout << "AFTER LAPLACE CORRECTION:" << std::endl;
    std::cout << "Non-match unique profile number = " << x_counts.size() << std::endl;
    std::cout << "Match unique profile number = " << m_counts.size() << std::endl;

    /////////////  End refactoring //////////

    // //////////////////////////////////////////
#if 1
    // TODO: Refactor this into `create_ratios` or something similar.
    //ratios = count of match / count of non-match;
    for (p = x_counts.begin(); p != x_counts.end(); ++p) {
        q = m_counts.find( p->first );
        if (q == m_counts.end()) 
            continue;
        else {
            ratio_map.insert(std::pair<SimilarityProfile, double>
                (p->first, 1.0 * q->second / p->second));
        }
    }
    //////////// End refactor
#else
    create_ratios();
#endif

    //////////////////
    // TODO: Refactor into `compare_profile_existence`
    uint32_t num_xcount_without_mcount = 0;
    uint32_t num_mcount_without_xcount = 0;

    SPCountsIndex::iterator pp = x_counts.begin();
    for (; pp != x_counts.end(); ) {

        // This needs to be a "contains" macro or something...
        if (ratio_map.find(pp->first) == ratio_map.end()) {
            x_counts.erase ( pp++ );
            ++num_xcount_without_mcount;
        } else {
            ++pp;
        }
    }

    SPCountsIndex::iterator qq = m_counts.begin();
    for (; qq != m_counts.end(); ) {

        // This needs to be a "contains" macro or something...
        if (ratio_map.find(qq->first) == ratio_map.end() ) {
            m_counts.erase ( qq++ );
            ++num_mcount_without_xcount;
        } else {
            ++qq;
        }
    }

    std::cout << num_xcount_without_mcount << " non-match similarity profiles and "
              << num_mcount_without_xcount << " match similarity profiles are discarded."
              << std::endl;

    if (num_xcount_without_mcount > 5 || num_mcount_without_xcount > 5) {

        std::cout << "WARNING: THIS STEP IS SKIPPED FOR DEBUG BUT SHALL BE "
                  << "ENABLED IN THE REAL PROGRAM, UNLESS SMOOTHING AND "
                  << "INTER-EXTRA-POLATIONS ARE AVAILABLE."
                  << std::endl;

        std::cout << "Discovered " << num_xcount_without_mcount
                  << " non-match similarity profiles that are not available in matched ones. "
                  << std::endl
                  << "And "
                  << num_mcount_without_xcount 
                  << " match similarity profiles that are not available in non-match ones."
                  << std::endl;
    }
    /////////////////// End refactor

    smooth();
    similarity_map.clear();
    is_ready = true;
}



/**
 * This should probably be `set_similarity_info` because
 * it doesn't return anything.
 *
 * Also, it doesn't actually set anything called `similarity_info`
 */
void
cRatioComponent::get_similarity_info() {

    positions_in_ratios.clear();
    positions_in_record.clear();
    attrib_names.clear();
    const Record & sample_record = Record::get_sample_record();
    static const string useless_group_label = "None";
    uint32_t ratios_pos = 0, record_pos = 0;

    vector<const Attribute*>::const_iterator p = sample_record.vector_pdata.begin();
    for (; p != sample_record.vector_pdata.end(); ++p) {

        const string & info = (*p)->get_attrib_group();
        bool comparator_activated = (*p)->is_comparator_activated();

        if (info == attrib_group && comparator_activated) {
            positions_in_ratios.push_back(ratios_pos);
            positions_in_record.push_back(record_pos);
            attrib_names.push_back(Record::get_column_names().at(record_pos));
        }

        if (info != useless_group_label && comparator_activated) ++ratios_pos;

        ++record_pos;
    }
}


/**
 * TODO: Describe the purpose of having all this code in the constructor.
 *
 * @param const vector <const cRatioComponent *> & component_pointer_vector
 *
 * @param const char * filename for the ratio_[\d].txt files, which are
 *   index by the number of the disambiguation round.
 *
 * @param const Record & rec
 */
cRatios::cRatios(const vector<const cRatioComponent *> & component_pointer_vector,
                 const char * filename,
                 const Record & rec) {

    std::cout << "Creating the final version ratios file ..." << std::endl;
    uint32_t ratio_size = 0;

    std::cout << "filename: " << filename << std::endl;

    vector< const cRatioComponent *>::const_iterator p = component_pointer_vector.begin(); 

    for (p; p != component_pointer_vector.end(); ++p ) {
        std::cout << " Size of Ratio Component = "
                  << (*p)->get_ratios_map().size() << std::endl;
        ratio_size += (*p)->get_component_positions_in_ratios().size();
    }

    attrib_names.resize(ratio_size, "Invalid Attribute");
    static const uint32_t impossible_value = 10000;
    vector<uint32_t> null_vect (ratio_size, impossible_value);

    final_ratios.insert(std::pair<vector<uint32_t>, double > (null_vect, 1) );
    x_counts.insert(std::pair<vector<uint32_t>, uint32_t > (null_vect, 0));
    m_counts.insert(std::pair<vector<uint32_t>, uint32_t > (null_vect, 0));

    p = component_pointer_vector.begin();
    for (p; p != component_pointer_vector.end(); ++p ) {
        More_Components(**p);
    }

// LEAVE THIS IN, this is something which may be necessary.
// It turns out that the following code often (usually? always?)
// segfaults because the final_ratios map is zeroed out in the
// More_Components function called above (which is a side effect).
#if 1
    if (final_ratios.size() > 0) {
      // now checking the final ratios
      //const vector < uint32_t > & firstline = final_ratios.begin()->first;
      map < const SimilarityProfile, double, SimilarityCompare>::const_iterator firstline = final_ratios.begin();
      //vector< uint32_t >::const_iterator k = firstline.begin(); 
      SimilarityProfile vec = (*firstline).first;
      SimilarityProfile::const_iterator k = vec.begin();
      //for (k; k < firstline.end(); ++k) {
      //for (k; k != firstline.end(); ++k) {
      for (k; k != vec.end(); ++k) {
        if (*k == impossible_value) {
          throw cRatioComponent::cException_Ratios_Not_Ready( "Final Ratios is not ready yet. ");
	}
      }
    }
#endif

    smooth();

    write_ratios_file(filename);
    x_counts.clear();
    m_counts.clear();
    similarity_map.clear();
}

cRatios::cRatios(const char * filename) {
    read_ratios_file(filename);
}


void
print_similarity(const SimilarityProfile & s) {

  std::cout << "From " << __FUNCTION__ << ", " << __FILE__ 
            << ":" << __LINE__ << std::endl;

  std::cout << "Size of sp: " << s.size();

  std::cout << "(";

  SimilarityProfile::const_iterator i = s.begin();
  for (i; i != s.end()-1; ++i) {
    std::cout << *i << ", ";
  }
  std::cout << *i;
  std::cout << ")" << std::endl;
}


void
print_value(uint32_t i) {
  std::cout << "From " << __FUNCTION__ << ", " << __FILE__ << ":" << __LINE__ << std::endl;
  std::cout << "Value: " << i << std::endl;
}


void
print_value(double d) {
  std::cout << "From " << __FUNCTION__ << ", " << __FILE__ << ":" << __LINE__ << std::endl;
  std::cout << "Value: " << d << std::endl;
}


void
print_map(SPRatiosIndex & m) {

  std::cout << "From " << __FUNCTION__ << ", " << __FILE__ << ":" << __LINE__ << std::endl;

  SPRatiosIndex::const_iterator mi = m.begin();
  for (mi; mi != m.end(); ++mi) {
    print_similarity((*mi).first);
    print_value((*mi).second);
  }
}


void
print_map(SPCountsIndex & m) {

  std::cout << "From " << __FILE__ << ":" << __LINE__ << std::endl;

  SPCountsIndex::const_iterator mi = m.begin();
  for (mi; mi != m.end(); ++mi) {
    print_similarity((*mi).first);
    print_value((*mi).second);
  }
}


void
cRatios::More_Components(const cRatioComponent & additional_component) {

    SPRatiosIndex temp_ratios;

    SPCountsIndex temp_x_counts, temp_m_counts;

    const vector<uint32_t> & temp_pos_in_rec = additional_component.get_component_positions_in_record();
    const vector<uint32_t> & positions_in_ratios = additional_component.get_component_positions_in_ratios();

    for (uint32_t k = 0; k < positions_in_ratios.size(); ++k) {
        attrib_names.at(positions_in_ratios.at(k)) = Record::get_column_names().at(temp_pos_in_rec.at(k) );
    }

    std::cout << "From cRatios::More_Components: final_ratios.size(): " << final_ratios.size()
	      << ", " << __FILE__ << ":" << __LINE__ << std::endl;

    map<SimilarityProfile, double, SimilarityCompare >::iterator p = final_ratios.begin();
    for (; p != final_ratios.end(); ++p) {

        vector < uint32_t > key = p->first;

        map<SimilarityProfile, double, SimilarityCompare >::const_iterator vv = additional_component.get_ratios_map().begin();
        for (; vv != additional_component.get_ratios_map().end(); ++vv) {

            for (uint32_t j = 0; j < vv->first.size(); ++j) {
                key.at(positions_in_ratios.at(j)) = vv->first.at(j);
            }

            temp_ratios.insert(std::pair<SimilarityProfile, double>(key, p->second * vv->second));

            temp_x_counts.insert(std::pair<vector<uint32_t>, uint32_t >(key,
                  this->x_counts.find(p->first)->second + additional_component.
                     get_x_counts().find(vv->first)->second));

            temp_m_counts.insert(std::pair<vector<uint32_t>, uint32_t >(key,
                  this->m_counts.find(p->first)->second + additional_component.
                     get_m_counts().find(vv->first)->second));
        }
    }

    final_ratios = temp_ratios;
    //print_map(final_ratios);
    x_counts = temp_x_counts;
    //print_map(x_counts);
    m_counts = temp_m_counts;
    //print_map(m_counts);
}


void
cRatios::write_ratios_file(const char * filename) const {

    std::ofstream::sync_with_stdio(false);
    std::cout << "Number of of ratios vectors = " << final_ratios.size()
              << " From: " << __FILE__ << ":" << __LINE__ <<std::endl;
    std::cout << "Saving the ratios in the file " << filename
              << " From: " << __FILE__ << ":" << __LINE__ <<std::endl;

    std::ofstream outfile(filename);

    // Firstname, etc. attributes in a top of file header row.   
    for (vector<string>::const_iterator p = attrib_names.begin(); p != attrib_names.end(); ++p) {
        outfile << *p << secondary_delim;
    }
    outfile << primary_delim << "VALUE" << '\n';

    // The actual values..
    map<SimilarityProfile, double, SimilarityCompare>::const_iterator q = final_ratios.begin();
    for (q; q != final_ratios.end(); ++q) {
        SimilarityProfile::const_iterator pint = q->first.begin(); 
        for (pint; pint != q->first.end(); ++pint) {
	    // Currently secondary_delim is ","
            outfile << *pint << secondary_delim;
	}
	// Currently primary_delim is "#"
        outfile << primary_delim << q->second << '\n';
    }
    std::cout << "Ratios file saved." << std::endl;
}


void
cRatios::read_ratios_file(const char * filename) {

    std::ifstream::sync_with_stdio(false);
    std::ifstream infile (filename);
    const uint32_t primary_delim_size = strlen(primary_delim);
    const uint32_t secondary_delim_size = strlen(secondary_delim);

    if (!infile.good()) throw cException_File_Not_Found(filename);

    string filedata;
    register size_t pos, prev_pos;
    getline(infile, filedata);
    pos = prev_pos = 0;

    while ((pos = filedata.find(secondary_delim, prev_pos)) != string::npos) {
        attrib_names.push_back(filedata.substr(prev_pos, pos - prev_pos));
        prev_pos = pos + secondary_delim_size;
    }

    SimilarityProfile key;
    while (getline(infile, filedata)) {

      // TODO: replace this with a templated callback
        key.clear();
        pos = prev_pos = 0;
        while ((pos = filedata.find(secondary_delim, prev_pos)) != string::npos) {
            key.push_back(atoi(filedata.substr(prev_pos, pos - prev_pos).c_str()));
            prev_pos = pos + secondary_delim_size;
        }

        pos = filedata.find(primary_delim, 0);
        pos += primary_delim_size;

        const double value = atof(filedata.substr(pos).c_str());

        final_ratios.insert(std::pair<SimilarityProfile, double>(key, value));
    }

    // TODO: This should probably not go here, invoke from calling function.
    Record::activate_comparators_by_name(attrib_names);

    std::cout << filename << " has been loaded as the final ratios file"<< std::endl;
    std::cout << "Resetting similarity profiles ... ..." << std::endl;
    std::cout << "-----Similarity Profiles reset.-------" << std::endl;
}


// TODO: Move this to record.cpp
const Record *
retrieve_record_pointer_by_unique_id(const string & uid,
                                     const RecordIndex & uid_tree) {

    RecordIndex::const_iterator cpm = uid_tree.find(uid);

    if (cpm == uid_tree.end()) {
        throw cException_Attribute_Not_In_Tree(uid.c_str());
    } else {
        return cpm->second;
    }
}

// TODO: See about moving this to record.cpp
void
create_btree_uid2record_pointer(RecordIndex & uid_tree,
                                const list<Record> & record_list,
                                const string & uid_name ) {


    uid_tree.clear();
    const uint32_t uid_index = Record::get_index_by_name(uid_name);
    cException_Vector_Data except(uid_name.c_str());

    RecordIndex::iterator pm;
    list<Record>::const_iterator record;
    for (record = record_list.begin(); record != record_list.end(); ++record ) {

        const Attribute * pattrib = record->get_attrib_pointer_by_index(uid_index);
        //if ( pattrib->get_data().size() != 1 ) throw except;
        const string & label = *pattrib->get_data().at(0);
        pm = uid_tree.find(label);

        if (pm != uid_tree.end()) {
            // This will throw on two records having the same Unique_Record_ID
	          // TODO: Document where Unique_Record_ID is assigned (probably
	          // in preprocessing consolidation.
            throw cException_Duplicate_Attribute_In_Tree(label.c_str());
	      }

        uid_tree.insert(std::pair<string, const Record *>(label, &(*record)));
    }
}
