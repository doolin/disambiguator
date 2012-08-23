
#ifndef PATENT_NEWCLUSTER_H
#define PATENT_NEWCLUSTER_H

#include "engine.h"
#include "clusterhead.h"

/**
 * Cluster objects are the molecules of disambiguation,
 * while Record objects are atoms of disambiguation.
 * Each cluster contains a cluster_head, a list of members, and some other
 * information. The aim of disambiguation is reorganize clusters
 * so that some can probably compound to bigger ones. Disambiguation
 * starts from smallest clusters that contain only one record, and
 * ends with clusters that contain some amount of records.
 */
class Cluster {

private:

  static const unsigned int invalid_year = 0;

  //ClusterHead m_info: cluster head of the cluster,
  //including the delegate and the cohesion of the cluster.
  ClusterHead m_info;

  //RecordList m_fellows: the list of members of the cluster.
  RecordList m_fellows;

  //bool m_mergeable: a boolean, indicating "*this" cluster
  //has been merged into others or not.
  bool m_mergeable;

  //bool m_usable: a boolean preventing misuse earlier than
  //fully prepared.
  bool m_usable;

  //static const cRatios * pratio: a pointer that points to a
  //cRatio object which contains a map of similarity profile to ratio
  static const cRatios * pratio;

 /**
  * static const map < const Record *, RecordList, cSort_by_attrib > * reference_pointer:
  * a pointer that points to a patent tree, which can be obtained in
  * a cBlocking_Operation_By_Coauthor object.
  */
  static const map < const Record *, RecordList, cSort_by_attrib > * reference_pointer;

  //Cluster & operator = ( const Cluster &): forbid the assignment operation.
  Cluster & operator = ( const Cluster &);

  //void find_representative():  to sets a component of cluster
  //head to be the record whose columns appear most frequently
  //among the all the members.
  void find_representative();


  unsigned int first_patent_year;

  unsigned int last_patent_year;

  set < const cLatitude * > locs;

  void update_locations();

  void update_year_range();

  unsigned int patents_gap( const Cluster & rhs) const;

  bool is_valid_year() const;

public:

  //  Cluster(const ClusterHead & info, const RecordList & fellows): constructor
  Cluster(const ClusterHead & info, const RecordList & fellows);

  //  ~Cluster() : destructor
  ~Cluster();

  //  Cluster ( const Cluster & rhs ): copy constructor
  Cluster ( const Cluster & rhs );

 /**
  *  void merge( Cluster & mergee, const ClusterHead & info):
  *  merge the "mergee" cluster into "*this", and set
  *  the cluster head of the new cluster to be info.
  */
   void merge( Cluster & mergee, const ClusterHead & info);

 /**
  * ClusterHead disambiguate(const Cluster & rhs, const double prior,
  * const double mutual_threshold) const:
  *  disambiguate "*this" cluster with rhs cluster,
  *  with the prior and mutual_threshold information.
  *  Returns a ClusterHead to tell whether the two clusters should
  *  be merged or not, and if yes, the cohesion of the new one.
  */
  ClusterHead disambiguate(const Cluster & rhs, const double prior,
      const double mutual_threshold) const;

  //static void set_ratiomap_pointer( const cRatios & r):
  //set the ratio map pointer to a good one.
  static void set_ratiomap_pointer( const cRatios & r) {pratio = &r;}

  //const RecordList & get_fellows() const:
  //get the members (actually it is reference to const) of the cluster.
  const RecordList & get_fellows() const {return m_fellows;}

  //const ClusterHead & get_cluster_head () const:
  //get the cluster head (const reference) of the cluster.
  const ClusterHead & get_cluster_head () const {return m_info;};

  //void insert_elem( const Record *): insert a new member into
  //the member list. This could potentially change the cluster head.
  void insert_elem(const Record *);

  //void self_repair(): call this if insertion of elements is done manually,
  //usually for a batch of record objects (not recommended).
  void self_repair();

  //static void set_reference_patent_tree_pointer(
  //const map < const Record *, RecordList, cSort_by_attrib > & reference_patent_tree):
  //set the patent tree pointer.
  static void set_reference_patent_tree_pointer(
      const map < const Record *, RecordList, cSort_by_attrib > & reference_patent_tree) {
    reference_pointer = & reference_patent_tree;
  }

 /**
  *  void change_mid_name(): to change pointers to abbreviated middle
  *  names to full middle names.  This step is controversial, as it
  *  actually changed the raw data. Or more correctly, it changed
  *  the pointers of the raw data.
  */
  void change_mid_name();

  void add_uid2uinv( map < const Record *, const Record *> & uid2uinv) const;
};


/**
 * cException_Empty_Cluster: an exception that may be used.
 */
class cException_Empty_Cluster : public cAbstract_Exception {
public:
  cException_Empty_Cluster(const char* errmsg): cAbstract_Exception(errmsg) {};
};


#endif /* PATENT_NEWCLUSTER_H */
