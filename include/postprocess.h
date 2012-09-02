
#ifndef PATENT_POSTPROCESS_H
#define PATENT_POSTPROCESS_H

#include "newcluster.h"


typedef list < Cluster > Cluster_Container;

class ClusterSet {

private:
    Cluster_Container consolidated;
    ClusterSet ( const ClusterSet &);

public:
    //ClusterSet & convert_from_ClusterInfo( const Cluster_Info * );
    const Cluster_Container & get_set() const {return consolidated;}
    Cluster_Container  & get_modifiable_set() { return consolidated; }
    ClusterSet () {}
    ~ClusterSet() {}
    void output_results( const char * ) const;

    void read_from_file (const char * filename,
                       const map <string, const Record*> & uid_tree);
};


void post_polish(ClusterSet & m,
                 map < const Record *, const Record *> & uid2uinv,
                 const PatentTree & patent_tree,
                 //const map < const Record *, RecordPList, cSort_by_attrib > & patent_tree,
                 const string & logfile);


#endif /* PATENT_POSTPROCESS_H */
