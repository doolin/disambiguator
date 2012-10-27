#ifndef PATENT_RECORD_PRIVATE_H
#define PATENT_RECORD_PRIVATE_H

#include <vector>
#include <string>

#include <stdint.h>

std::vector<std::string> parse_column_names (std::string line);

std::vector<uint32_t> create_column_indices(
    std::vector<std::string> requested_columns,
    std::vector<std::string> total_col_names);

#endif // PATENT_RECORD_PRIVATE_H
