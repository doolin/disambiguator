
#include <string>
#include <fstream>

#include <cppunit/TestCase.h>

#include <training.h>

#include "testutils.h"

extern "C" {
  #include "strcmp95.h"
}

using std::string;
using std::pair;


class RarenamesTest : public CppUnit::TestCase {

public:
  RarenamesTest(string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
  }

 /**
  * The name_compare function builds a similarity
  * weight.
  */
  void test_rarename() {
    //find_rare_names_v2(vec_pdest, source);
  }

  void test_choose_rare_words() {

    WordCounter wc;
    wc.insert (pair<string, WordCounts> (string("foo"),   WordCounts(1,2)));
    wc.insert (pair<string, WordCounts> (string("bar"),   WordCounts(1,88)));
    wc.insert (pair<string, WordCounts> (string("barbar"),WordCounts(3,88)));
    wc.insert (pair<string, WordCounts> (string("baz"),   WordCounts(1,200)));
    wc.insert (pair<string, WordCounts> (string("quux"),  WordCounts(5,2)));
    wc.insert (pair<string, WordCounts> (string("red"),   WordCounts(5,88)));
    wc.insert (pair<string, WordCounts> (string("black"), WordCounts(5,200)));

    std::set<std::string> rarewords;
    choose_rare_words(wc, rarewords);

    CPPUNIT_ASSERT(0 == rarewords.count("foo"));
    CPPUNIT_ASSERT(1 == rarewords.count("bar"));
    CPPUNIT_ASSERT(1 == rarewords.count("barbar"));
    CPPUNIT_ASSERT(0 == rarewords.count("baz"));
    CPPUNIT_ASSERT(0 == rarewords.count("quux"));
    CPPUNIT_ASSERT(0 == rarewords.count("red"));
    CPPUNIT_ASSERT(0 == rarewords.count("black"));
  }


  void runTests() {
    test_rarename();
    test_choose_rare_words();
  }

};


void test_rarenames() {

  RarenamesTest * rt = new RarenamesTest(std::string("Rare names unit tests"));
  rt->runTests();
  delete rt;
}


#ifdef test_rarenames_STANDALONE
int
main(int, char **) {
  test_rarenames();
  return 0;
}
#endif
