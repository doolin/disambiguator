
#ifndef PATENT_TYPEDEFS_H
#define PATENT_TYPEDEFS_H

#include <vector>
#include <string>

typedef std::vector<std::string> Label;

// SimilarityProfile will probably get moved back
// to the ratios and smoothing code once that us
// cleaned up and refactored more.
typedef std::vector < unsigned int > SimilarityProfile;

#endif /* PATENT_TYPEDEFS_H */
