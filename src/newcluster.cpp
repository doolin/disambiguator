

#include "newcluster.h"

//static members initialization.
const cRatios * Cluster::pratio = NULL;
const map < const Record *, RecordPList, cSort_by_attrib > * Cluster::reference_pointer = NULL;


/**
 * Aim: constructor of Cluster objects.
 */
Cluster::Cluster(const ClusterHead & info, const RecordPList & fellows)
		: m_info(info), m_fellows(fellows), m_mergeable(true), m_usable(true) {

  // No. Wrong. This is just bad design. You just don't require static
  // variables to be set before constructing an instance of a class. This is
  // amazingly bad practice. If this class **NEEDS** this static member
  // set as a precondition, then the class is designed WRONG. By definition,
  // WRONG. This is WRONG DESIGN.
  // TODO: Find out why and where patent tree reference pointer is needed,
  // why it's used, and figure out a way to get rid of this exception.
	if (NULL == reference_pointer) {
		throw cException_Other("Critical Error: Patent tree reference pointer is not set yet.");
  }

  // TODO: Refactor all this into an init() method (or init_private)
  // such that it can be called from this constructor, and possibly
  // as its own method.
	this->first_patent_year = invalid_year;
	this->last_patent_year = invalid_year;
  // This is also wrong design. Don't do work in constructors. Not ever.
	this->update_year_range();
	this->update_locations();
}


/**
 * Aim: merge the mergee object into "*this", with the new cluster
 * head = info ( Actually only the cohesion is used,
 * because the delegate will be reset by find_representative).
 *
 * Algorithm: put the mergee's members into "*this" object,
 * and set mergee's signal to false.
 * And then call find_representative.
 */
void
Cluster::merge(Cluster & mergee, const ClusterHead & info) {

	if (this->m_mergeable == false) {
		throw cException_Empty_Cluster("Merging error: merger is empty.");
  }

	if (mergee.m_mergeable == false) {
		throw cException_Empty_Cluster("Merging error: mergEE is empty.");
  }

	static const uint32_t rec_size = Record::record_size();

	for (uint32_t i = 0 ; i < rec_size; ++i) {

		list < const Attribute ** > l1;
		for (RecordPList::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p) {
			l1.push_back(const_cast < const Attribute ** > (&(*p)->get_attrib_pointer_by_index(i)));
		}

		list < const Attribute ** > l2;
		for (RecordPList::const_iterator p = mergee.m_fellows.begin(); p != mergee.m_fellows.end(); ++p) {
			l2.push_back(const_cast < const Attribute ** > (&(*p)->get_attrib_pointer_by_index(i)));
		}
		attrib_merge(l1, l2);
	}

	this->m_info = info;
	this->m_fellows.insert(m_fellows.end(), mergee.m_fellows.begin(), mergee.m_fellows.end());
	mergee.m_fellows.clear();
	mergee.locs.clear();

	this->find_representative();
	this->update_year_range();
	this->update_locations();
	mergee.m_mergeable = false;
}


/**
 * Aim: To change the pointer to abbreviated middle names to the full names.
 *
 * Algorithm: build a binary tree: unique last name -> longest middle
 * name pointer. Then traverse the whole record members to
 * update the binary tree. Finally, traverse the record members again,
 * look up their last names and update their
 * middle name pointers to the corresponding pointer.
 * ATTENTION: for records without middle names (ie. the pointer of middle
 * name points to an empty string ), the modification will
 * NOT take place. This is because it is very risky, as we have no
 * information about the real middle name.
 */
void
Cluster::change_mid_name()  {

	// The folowing step actually changes the raw data. Changes the
  // abbreviated middlename to a longer one if possible.
	if (!cMiddlename::is_enabled()) return;

	static const uint32_t midname_index = Record::get_index_by_name(cMiddlename::static_get_class_name());
	static const uint32_t lastname_index = Record::get_index_by_name(cLastname::static_get_class_name());
	map < const Attribute *, const Attribute *> last2mid;
	map < const Attribute *, const Attribute * >::iterator q;

	for ( RecordPList::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const Attribute * pl = (*p)->get_attrib_pointer_by_index(lastname_index);
		const Attribute * pm = (*p)->get_attrib_pointer_by_index(midname_index);
		q = last2mid.find(pl);
		if ( q == last2mid.end() )  {
			if ( pm->is_informative() ) {
				last2mid.insert( std::pair< const Attribute *, const Attribute *> (pl, pm) );
			}

		} else {
			const uint32_t old_size = q->second->get_data().at(0)->size();
			const uint32_t new_size = pm->get_data().at(0)->size();
			if ( new_size > old_size )
				q->second = pm;
		}
	}

	map < const Attribute *, const Attribute * >::const_iterator cq;

  RecordPList::iterator p = this->m_fellows.begin();
	for (; p != this->m_fellows.end(); ++p) {
		const Attribute * pl = (*p)->get_attrib_pointer_by_index(lastname_index);
		const Attribute * const & pm = (*p)->get_attrib_pointer_by_index(midname_index);
		cq = last2mid.find(pl);

		//skip empty middle names.
		if (pm->is_informative() && pm != cq->second) {
			cq->second->add_attrib(1);
			pm->reduce_attrib(1);
			const Attribute* & rpm = const_cast < const Attribute* & > (pm);
			rpm = cq->second;
		}
	}
	// end of modification
}



//destructor.
Cluster::~Cluster() {}


//copy constructor
Cluster::Cluster( const Cluster & rhs ) : m_info(rhs.m_info), m_fellows(rhs.m_fellows), m_mergeable(true),
		first_patent_year ( rhs.first_patent_year ), last_patent_year ( rhs.last_patent_year ) {

	if (rhs.m_mergeable == false) {
		throw cException_Other("Cluster Copy Constructor error.");
  }
}


/**
 * Aim: to compare "*this" with rhs, and check whether the clusters
 * should merge. If it is a merge, the returned cluster head consists
 * of a non-NULL Record pointer and a cohesion value; if it is nor
 * a merge, the returned cluster head is composed of a NULL Record
 * pointer, and the cohesion value is useless then. Algorithm: Check
 * if some special cases should be dealt first. Otherwise, call
 * disambiguate_by_set in engine.cpp
 */
ClusterHead
Cluster::disambiguate(const Cluster & rhs,
                      const double prior,
                      const double mutual_threshold) const {

	static const uint32_t country_index = Record::get_index_by_name(cCountry::static_get_class_name());
	static const string asian_countries[] = {"JP"};
	static const double asian_threshold = 0.99;
	static const double max_threshold = 0.999;


	if (pratio == NULL) {
		throw cException_Other("Critical: ratios map is not set yet.");
  }

	if (this->m_mergeable == false) {
		throw cException_Empty_Cluster("Comparison error: lhs is empty.");
	}

	if (rhs.m_mergeable == false) {
		throw cException_Empty_Cluster("Comparison error: rhs is empty.");
	}

	double threshold = mutual_threshold;
	const Attribute * this_country = this->m_info.m_delegate->get_attrib_pointer_by_index(country_index);
	const Attribute * rhs_country = rhs.m_info.m_delegate->get_attrib_pointer_by_index(country_index);

	for (uint32_t i = 0; i < sizeof(asian_countries)/sizeof(string); ++i) {
		if (this_country == rhs_country && *this_country->get_data().at(0) == asian_countries[i]) {
			threshold = asian_threshold > mutual_threshold ? asian_threshold : mutual_threshold;
			break;
		}
	}

	uint32_t gap = this->patents_gap(rhs);
	bool location_penalize = false;
	const uint32_t common_locs = num_common_elements(this->locs.begin(), this->locs.end(), rhs.locs.begin(), rhs.locs.end(), 1 );

	if (gap == 0 && common_locs == 0) location_penalize = true;

	static const uint32_t max_gap = 20;

	if (gap > max_gap) gap = max_gap;

	double prior_to_use = prior;
	double threshold_to_use = threshold;

	threshold_to_use = threshold + (max_threshold - threshold) * gap / max_gap;

	if (prior_to_use == 0) prior_to_use = 0.01;

	if (location_penalize) {
		const double t = threshold_to_use + (max_threshold - threshold_to_use) / 2;
		threshold_to_use = t;
	}

	if ( threshold_to_use > max_threshold )
		threshold_to_use = max_threshold;

  // TODO: CAVEAT: cohesion is what is actually returned here, associated with
  // a record from the cluster being examined.
 	std::pair<const Record *, double > ans (disambiguate_by_set (this->m_info.m_delegate,
                                                               this->m_fellows,
                                                               this->m_info.m_cohesion,
												                                       rhs.m_info.m_delegate,
                                                               rhs.m_fellows,
                                                               rhs.m_info.m_cohesion,
													                                     prior_to_use,
                                                              *pratio,
                                                               threshold_to_use));

  // TODO: CAVEAT: cohesion
	return ClusterHead(ans.first, ans.second);

}


/**
 * Aim: insert a new record pointer to the member list.
 */
void
Cluster::insert_elem( const Record * more_elem) {

	this->m_fellows.push_back(more_elem);
	m_usable = false;
}


/**
 * Aim: to repair a cluster.
 */
void
Cluster::self_repair() {

	const uint32_t rec_size = Record::record_size();
	for ( uint32_t i = 0 ; i < rec_size; ++i ) {

		list < const Attribute ** > l1;
		list < const Attribute ** > l2;
		RecordPList::const_iterator p1 = this->m_fellows.begin();

		if (p1 == this->m_fellows.end()) break;

		RecordPList::const_iterator q2 = p1;
		++q2;
		l2.push_back( const_cast < const Attribute ** > ( &(*p1)->get_attrib_pointer_by_index(i)  )   );

		while (q2 != this->m_fellows.end()) {
			l1.push_back(const_cast < const Attribute ** > ( &(*p1)->get_attrib_pointer_by_index(i)  )   );
			l2.pop_front();
			l2.push_back(const_cast < const Attribute ** > ( &(*q2)->get_attrib_pointer_by_index(i)  )   );
			attrib_merge(l1, l2);
			++p1;
			++q2;
		}
	}

	//if it has not been merged before and m_usable is false, reset to usable.
	this->update_year_range();
	this->update_locations();

	if (m_usable == false && !m_fellows.empty()) m_usable = true;
}


// Memory might be an issue here.
// This is part of Cluster::find_representative
vector<uint32_t>
make_indice(const string columns[], const uint32_t numcols) {

	vector<uint32_t> indice;
	for (uint32_t i = 0; i < numcols; ++i) {
		indice.push_back(Record::get_index_by_name(columns[i]));
  }
  return indice;
}


/**
 * Aim: to find a representative/delegate for a cluster.
 *
 * Algorithm: for each specified column, build a binary map of
 * const Attribute pointer -> uint32_t (as a counter).
 * Then traverse the whole cluster and fill in the counter.
 * Finally, get the most frequent.
 */
void
Cluster::find_representative()  {

  // TODO: This smells like something which ought to be in
  // a configuration variable.
	static const string useful_columns[] = {
    cFirstname::static_get_class_name(),
    cMiddlename::static_get_class_name(),
    cLastname::static_get_class_name(),
    cLatitude::static_get_class_name(),
    cAssignee::static_get_class_name(),
    cCity::static_get_class_name(),
    cCountry::static_get_class_name()
  };
	static const uint32_t numcols = sizeof(useful_columns)/sizeof(string);

#if 1
  // TODO: Refactor this block. ////////////////////////////////
	vector<uint32_t> indice;
	for (uint32_t i = 0; i < numcols; ++i) {
		indice.push_back(Record::get_index_by_name(useful_columns[i]));
  }
  ///////////////////// End refactor  ////////////////////////////////
#else
	vector<uint32_t> indice = make_indice(useful_columns, numcols);
#endif

	vector < map<const Attribute *, uint32_t> > tracer(numcols);
	for (RecordPList::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p) {
		for (uint32_t i = 0 ; i < numcols; ++i) {
			const Attribute * pA = (*p)->get_attrib_pointer_by_index(indice.at(i));
			++tracer.at(i)[pA];
		}
	}


  // TODO: Refactor this block. ////////////////////////////////
	vector<const Attribute *> most;
	for (uint32_t i = 0; i < numcols ; ++i) {

		const Attribute * most_pA = NULL;
		uint32_t most_cnt = 0;

		for (map < const Attribute *, uint32_t >::const_iterator p = tracer.at(i).begin(); p != tracer.at(i).end(); ++p) {
			if (p->second > most_cnt) {
				most_cnt = p->second;
				most_pA = p->first;
			}
		}
		most.push_back(most_pA);
	}
  ///////////////////// End refactor  ////////////////////////////////


  // TODO: Refactor into max_attribute_count or something
	uint32_t m_cnt = 0;
  // TODO: change to nullptr.
	const Record * mp = NULL;

	for (RecordPList::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p) {
		uint32_t c = 0;

		for (uint32_t i = 0 ; i < numcols; ++i) {
			const Attribute * pA = (*p)->get_attrib_pointer_by_index(indice.at(i));
			if (pA == most.at(i)) ++c;
		}

		if (c > m_cnt) {
			m_cnt = c;
			mp = *p;
		}

	}

  // return mp;// from refactor
  // /// End refactor

	this->m_info.m_delegate = mp;
}


void
Cluster::update_year_range() {

	static const uint32_t appyearindex = Record::get_index_by_name(cApplyYear::static_get_class_name());

	RecordPList::const_iterator rlit = this->m_fellows.begin();
	for (; rlit != this->m_fellows.end(); ++rlit ) {

		const Attribute * pAttribYear = (*rlit)->get_attrib_pointer_by_index(appyearindex);
    // This is segfaulting on test data
		const string * py = pAttribYear->get_data().at(0);
		uint32_t year = atoi ( py->c_str());

		if (year > 2100 || year < 1500) {
			//(*p)->print();
			//throw cException_Other("Application year error.");
			continue;
		}

		if ( this->is_valid_year() ) {
			if ( year > this->last_patent_year )
				this->last_patent_year = year;
			if ( year < this->first_patent_year )
				this->first_patent_year = year;
		}
		else {
			this->first_patent_year = year;
			this->last_patent_year = year;
		}
	}

}


bool
Cluster::is_valid_year() const {

	if (this->first_patent_year == invalid_year || this->last_patent_year == invalid_year )
		return false;
	else
		return true;
}


uint32_t
Cluster::patents_gap(const Cluster & rhs) const {

	if (!this->is_valid_year() || !rhs.is_valid_year()) return 0;

	uint32_t x = 0;

  // ABS...? AYFKM...?
	if (this->first_patent_year > rhs.last_patent_year)
		x = this->first_patent_year - rhs.last_patent_year;
	else if  (this->last_patent_year < rhs.first_patent_year)
		x = rhs.first_patent_year - this->last_patent_year;

	if (x > 500) {
		throw cException_Other("Patent gap error.");
  }

	return x;
}


void
Cluster::update_locations() {

	locs.clear();
	static const uint32_t latindex = Record::get_index_by_name(cLatitude::static_get_class_name());

  RecordPList::const_iterator p = this->m_fellows.begin();
	for (; p != this->m_fellows.end(); ++p) {

		const Attribute * pA = (*p)->get_attrib_pointer_by_index(latindex);
		const cLatitude * pAttribLat = dynamic_cast< const cLatitude *>(pA);

		if (pAttribLat == 0) {
			(*p)->print();
			std::cerr << "Data type is " << typeid(*pA).name() << std::endl;
			throw cException_Other("bad cast from cAttrib to cLatitude. Cluster::update_location error.");
		}

		if (pAttribLat->is_informative())
      locs.insert(pAttribLat);

	}
}


void
Cluster::add_uid2uinv(Uid2UinvTree & uid2uinv ) const {

	Uid2UinvTree::iterator q;

  RecordPList::const_iterator p = this->m_fellows.begin();
	for (; p != m_fellows.end(); ++p) {

		q = uid2uinv.find(*p);
		if (q != uid2uinv.end()) {
			throw cException_Other("Add uid: already exists.");
    } else {
			uid2uinv.insert(std::pair<const Record *, const Record *>(*p, this->m_info.m_delegate));
    }
	}
}
