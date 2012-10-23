#include <iostream>

#include "colortest.h"
#include "testutils.h"


void
describe(const char * indent, const char * description, const char * COLOR) {

    std::cout << indent << COLOR << description << COLOR_RESET
              << std::endl;
}

void
describe_test(const char * indent, const char * description) {
  describe(indent, description, DESCCOLOR);
}


void
describe_pass(const char * indent, const char * description) {
  describe(indent, description, PASSCOLOR);
}


void
describe_fail(const char * indent, const char * description) {
  describe(indent, description, FAILCOLOR);
}

