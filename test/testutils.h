#ifndef PATENT_TESTUTILS_H
#define PATENT_TESTUTILS_H

#define DESCCOLOR COLOR45
#define PASSCOLOR COLOR119
#define FAILCOLOR COLOR124

#define INDENT0 ""
#define INDENT2 "  "
#define INDENT4 "     "
#define INDENT6 "       "

typedef void (*Describer)(const char *, const char *);

void describe_test(const char * indent, const char * description);
void describe_pass(const char * indent, const char * description);
void describe_fail(const char * indent, const char * description);

class TestUtils {
};



#endif // PATENT_TESTUTILS_H
