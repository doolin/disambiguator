
// https://bitbucket.org/doolin/disambiguator/src/450cd6c85791/src/training.cpp#cl-501

#include <string>
#include <fstream>

#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include "training.h"

extern "C" {
  #include "strcmp95.h"
}

using std::string;


class RarenamesTest : public CppUnit::TestCase {

//    find_rare_names_v2(const vector < cGroup_Value * > &vec_pdest,
//                       const list< const Record* > & source )
//  const vector < cGroup_Value * > & vec_pdest;
//  const list< const Record * > & source;

public:
  RarenamesTest(std::string name) : CppUnit::TestCase(name) {}



 /**
  * The name_compare function builds a similarity
  * weight.
  */
  void test_rarename() {

    //find_rare_names_v2(vec_pdest, source);
    CPPUNIT_ASSERT(1 == 1);
  }

};


void test_rarenames() {

  RarenamesTest * rt = new RarenamesTest(std::string("initial test"));
  rt->test_rarename();
  delete rt;
}


#ifdef test_rarenames_STANDALONE
int
main(int argc, char ** argv) {
  test_rarenames();
  return 0;
}
#endif
