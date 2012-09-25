#include <iostream>

#include "colortest.h"
#include "testutils.h"

void
describe_test(const char * indent, const char * description) {

    std::cout << indent << TESTCOLOR << description << COLOR_RESET
              << std::endl;
}
