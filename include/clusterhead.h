#ifndef PATENT_CLUSTERHEAD_H
#define PATENT_CLUSTERHEAD_H

/**
 * ClusterHead:
 * This class contains two pieces of information about
 * a cluster: its delegate and its cohesion.
 */
class ClusterHead {

public:

 /** const Record * m_delegate: the delegate (representative) of a cluster.
  * Usually this pointer contains the most frequently occurring information.
  */
  const Record * m_delegate;

 /**
  *  double m_cohesion: the cohesion of a cluster, meaning the probability
  *  for the members of the cluster to be of the same inventor.
  */
  double m_cohesion;

 /**
  *  ClusterHead(const Record * const p, const double c): constructor
  */
  ClusterHead(const Record * const representative, const double cohesion)
              : m_delegate(representative), m_cohesion(cohesion) {};

 /**
  * ClusterHead ( const ClusterHead & rhs): copy constructor
  */
  ClusterHead (const ClusterHead & rhs)
               : m_delegate(rhs.m_delegate), m_cohesion(rhs.m_cohesion) {}
};

#endif /* PATENT_CLUSTERHEAD_H */
