#ifndef PATENT_TESTUTILS_H
#define PATENT_TESTUTILS_H

#include <algorithm>

#include <cppunit/TestCase.h>

#define DESCCOLOR COLOR45
#define PASSCOLOR COLOR119
#define FAILCOLOR COLOR124
#define PENDINGCOLOR COLOR166

#define INDENT0 ""
#define INDENT2 "  "
#define INDENT4 "     "
#define INDENT6 "       "


typedef const char * Description;
typedef void (*Describer)(const char *, const char *);

void describe_test(const char * indent, const char * description);
void describe_pass(const char * indent, const char * description);
void describe_fail(const char * indent, const char * description);
void describe_pending(const char * indent, const char * description);

#define DO_SPEC [](Description d)->bool
#define DO_SPEC_THIS [this](Description d)->bool
#define DO_SPEC_HANDLE [&](Description d)->bool

class Spec {

public:

  char buf[512];

  void it (Description desc, std::function<bool(Description)> test) {

    Describer d;
    sprintf(buf, desc);
    try {
      CPPUNIT_ASSERT(test(desc));
      d = describe_pass;
    } catch (CppUnit::Exception e) {
      d = describe_fail;
    }
    d(INDENT4, buf);
  }

  void xit (Description desc, std::function<bool(Description)> test) {

    describe_pending(INDENT4, desc);
  }

};

class TestUtils {
};


#endif // PATENT_TESTUTILS_H
