#include <string>
#include <vector>
#include <iostream>

//#include <cppunit/Portability.h>
//#include <cppunit/portability/CppUnitSet.h>
//#include <cppunit/extensions/TestFactory.h>
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

class SimilarityTest : public CppUnit::TestCase { 

public: 
  SimilarityTest(std::string name) : CppUnit::TestCase(name) {}

  void extract_first_word() {
  }

  void runTest() {
    extract_first_word();
  }
};

#ifdef test_similarity_STANDALONE
int
main(int, char **) {

  SimilarityTest * st = new SimilarityTest(std::string("initial test"));
  st->runTest();
  delete st;
  return 0;
}
#endif
