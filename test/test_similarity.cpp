#include <string>
#include <vector>
#include <iostream>

#include <cppunit/TestCase.h>

//#include <disambiguation.h>
//#include <engine.h>

#if 0

Comparison function scoring: LEFT/RIGHT=LEFT VS RIGHT

1) Firstname: 0-6. Factors: # of token and similarity between tokens
Totally different: THOMAS ERIC/RICHARD JACK EVAN
ONE NAME MISSING: THOMAS ERIC/(NONE)
THOMAS ERIC/ THOMAS JOHN ALEX
LEE RON ERIC/LEE ALEX ERIC
//No space match but raw names don't: JOHNERIC/JOHN ERIC. Short name vs long name: ERIC/ERIC THOMAS
ALEX NICHOLAS/ALEX NICHOLAS TAKASHI
ALEX NICHOLAS/ALEX NICHOLA (Might be not exactly the same but identified the same by jaro-wrinkler)

2) Lastname: 0-6 Factors: # of token and similarity between tokens
Totally different: ANDERSON/DAVIDSON
ONE NAME MISSING: ANDERSON/(NONE)
First part non-match: DE AMOUR/DA AMOUR
VAN DE WAALS/VAN DES WAALS
DE AMOUR/DEAMOUR
JOHNSTON/JOHNSON
DE AMOUR/DE AMOURS

3) Midname: 0-4 (THE FOLLOWING EXAMPLES ARE FROM THE COLUMN FIRSTNAME, SO FIRSTNAME IS INCLUDED)
THOMAS ERIC/JOHN THOMAS
JOHN ERIC/JOHN (MISSING)
THOMAS ERIC ALEX/JACK ERIC RONALD
THOMAS ERIC RON ALEX EDWARD/JACK ERIC RON ALEX LEE
THOMAS ERIC/THOMAS ERIC LEE

4) Assignee: 0-8
DIFFERENT ASGNUM, TOTALY DIFFERENT NAMES (NO single common word)
DIFFERENT ASGNUM, One name missing
DIFFERENT ASGNUM, Harvard University Longwood Medical School /Dartmouth Hitchcock Medical Center
DIFFERENT ASGNUM, Harvard University President and Fellows / Presidents and Fellow of Harvard
DIFFERENT ASGNUM, Harvard University / Harvard University Medical School
DIFFERENT ASGNUM, Microsoft Corporation/Microsoft Corporated
SAME ASGNUM, COMPANY SIZE>1000
SAME ASGNUM, 1000>SIZE>100
SAME ASGNUM, SIZE<100

5) CLASS: 0-4
# OF COMMON CLASSES. MISSING=1

6) COAUTHERS 0-10
# OF COMMON COAUTHORS

7) DISTANCE: 0-7 FACTORS: LONGITUDE/LATITUDE, STREET ADDRESS
TOTALLY DIFFERENT
ONE IS MISSING
75<DISTANCE < 100KM
50<DISTANCE < 75
10<DISTANCE < 50
DISTANCE < 10
DISTANCE < 10 AND STREET MATCH BUT NOT IN US, OR DISTANCE < 10 AND IN US BUT STREET NOT MATCH
Street match and in USA

#endif

#include <typedefs.h>
#include <ratios.h>

#include "testutils.h"

SimilarityProfile
csp(uint32_t s1, uint32_t s2) {
  SimilarityProfile sp;
  sp.push_back(s1);
  sp.push_back(s2);
  return sp;
}


class SimilarityTest : public CppUnit::TestCase {

private:
    SimilarityProfile sp, max, min;

public:
  SimilarityTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());

    max.push_back(2);
    max.push_back(2);
    min.push_back(0);
    min.push_back(0);
  }

  void test_sp2index() {

    sp.push_back(1);
    sp.push_back(1);
    uint32_t index = sp2index(sp, min, max);
    CPPUNIT_ASSERT(4 == index);
    //std::cout << "sp2index: " << index << std::endl;

    sp.clear();
    sp.push_back(1);
    sp.push_back(0);
    index = sp2index(sp, min, max);
    CPPUNIT_ASSERT(3 == index);
    //std::cout << "sp2index: " << index << std::endl;
    describe_pass(INDENT2, "Retrieved index given similarity profile");
  }

  void test_index2sp() {

    SimilarityProfile sp = index2sp(4, min, max);
    CPPUNIT_ASSERT(sp == csp(1,1));
    //print_similarity(sp);
    sp = index2sp(3, min, max);
    CPPUNIT_ASSERT(sp == csp(1,0));
    //print_similarity(sp);
    describe_pass(INDENT2, "Retrieved similarity profile given index");
  }



  void test_cpp11_syntax() {
    vector<int> sp1 = {1, 2, 3};
    vector<int> sp2({1, 2, 3});
    vector<int> sp3{1, 2, 3};
  }


  void runTest() {
    test_sp2index();
    test_index2sp();
    test_cpp11_syntax();
  }
};


void
test_similarity() {

  SimilarityTest * st = new SimilarityTest(std::string("Similarity test"));
  st->runTest();
  delete st;
}


#ifdef test_similarity_STANDALONE
int
main(int, char **) {

  test_similarity();
  return 0;
}
#endif
