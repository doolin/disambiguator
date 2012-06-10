
#ifndef PATENT_TYPEDEFS_H
#define PATENT_TYPEDEFS_H

#include <vector>
#include <string>

typedef std::vector<std::string> Label;

// SimilarityProfile will probably get moved back
// to the ratios and smoothing code once that us
// cleaned up and refactored more.
typedef std::vector < unsigned int > SimilarityProfile;

//asgdetail consists of assignee number and its patent counts.
typedef std::pair<std::string, unsigned int> asgdetail;


#endif /* PATENT_TYPEDEFS_H */
