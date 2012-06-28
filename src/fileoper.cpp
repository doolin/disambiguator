
#include <fstream>
#include <cstring>


#include "fileoper.h"
#include "engine.h"
#include "cluster.h"


const Record *
retrieve_record_pointer_by_unique_id(const string & uid,
                                     const map <string, const Record*> & uid_tree) {

    map <string, const Record *>::const_iterator cpm = uid_tree.find(uid);
    if ( cpm == uid_tree.end())
        throw cException_Attribute_Not_In_Tree(uid.c_str());
    else
        return cpm->second;
}


void
create_btree_uid2record_pointer(map<string, const Record *> & uid_tree,
                                const list<Record> & record_list,
                                const string & uid_name ) {


    uid_tree.clear();
    const unsigned int uid_index = Record::get_index_by_name(uid_name);
    cException_Vector_Data except(uid_name.c_str());

    map <string, const Record *>::iterator pm;
    list<Record>::const_iterator record;
    for (record = record_list.begin(); record != record_list.end(); ++record ) {

        const Attribute * pattrib = record->get_attrib_pointer_by_index(uid_index);
        //if ( pattrib->get_data().size() != 1 ) throw except;
        const string & label = *pattrib->get_data().at(0);
        pm = uid_tree.find(label);

        if ( pm != uid_tree.end()) {
            // This will throw on two records having the same Unique_Record_ID
	    // TODO: Document where Unique_Record_ID is assigned (probably
	    // in preprocessing consolidation.
            throw cException_Duplicate_Attribute_In_Tree(label.c_str());
	}

        uid_tree.insert(std::pair< string, const Record *>(label, &(*record)));
    }
}
