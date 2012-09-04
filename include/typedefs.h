
#ifndef PATENT_TYPEDEFS_H
#define PATENT_TYPEDEFS_H

#include <vector>
#include <string>

#include <stdint.h>

typedef std::vector<std::string> Label;

// SimilarityProfile will probably get moved back
// to the ratios and smoothing code once that us
// cleaned up and refactored more.
typedef std::vector<unsigned int> SimilarityProfile;

//asgdetail consists of assignee number and its patent counts.
typedef std::pair<std::string, unsigned int> asgdetail;

// Use these for dealing with SimilarityProfile counts.
typedef uint32_t sp_count_t;
typedef sp_count_t m_count_t;
typedef sp_count_t n_count_t;


#endif /* PATENT_TYPEDEFS_H */
