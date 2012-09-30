#include <iostream>

#include "colortest.h"
#include "testutils.h"

/*
#undef TESTCOLOR
#define TESTCOLOR ""

#undef COLOR_RESET
#define COLOR_RESET ""
*/

void
describe_test(const char * indent, const char * description) {

    std::cout << indent << TESTCOLOR << description << COLOR_RESET
              << std::endl;
}
