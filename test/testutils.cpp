#include <iostream>
#include <algorithm>
#include <string>

#include "colortest.h"
#include "testutils.h"

using std::string;

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
describe_pending(const char * indent, const char * description) {
  string pending("(PENDING) ");
  string d(description);
  pending = pending + d;
  describe(indent, pending.c_str() , PENDINGCOLOR);
}


void
describe_pass(const char * indent, const char * description) {
  describe(indent, description, PASSCOLOR);
}


void
describe_fail(const char * indent, const char * description) {
  describe(indent, description, FAILCOLOR);
}

