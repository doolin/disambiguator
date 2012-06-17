
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>
#include <vector>


#include <attribute.h>


class CompareTest : public CppUnit::TestCase { 

public: 
  CompareTest( std::string name ) : CppUnit::TestCase( name ) {}

  void check_less_than() {
    std::vector <unsigned int> s1;
    s1.push_back(1);
    std::vector <unsigned int> s2;
    s2.push_back(2);
    SimilarityCompare sc;
    bool checkval = sc(s1, s2);
    //std::cout << "checkval: " << checkval << std::endl;
    CPPUNIT_ASSERT(checkval == true);
  }

  void check_greater_than() {
    std::vector <unsigned int> s1;
    s1.push_back(2);
    std::vector <unsigned int> s2;
    s2.push_back(1);
    SimilarityCompare sc;
    bool checkval = sc(s1, s2);
    //std::cout << "checkval: " << checkval << std::endl;
    CPPUNIT_ASSERT(checkval == false);
  }

  void check_equals_to() {
    std::vector <unsigned int> s1;
    s1.push_back(1);
    std::vector <unsigned int> s2;
    s2.push_back(1);
    SimilarityCompare sc;
    bool checkval = sc(s1, s2);
    //std::cout << "checkval: " << checkval << std::endl;
    CPPUNIT_ASSERT(checkval == false);
  }

  void runTest() {
    check_less_than();
    check_greater_than();
    check_equals_to();
  }
};


int
main(int argc, char ** argv) {

  CompareTest * ct = new CompareTest(std::string("SimilarityCompare test"));
  ct->runTest();
  delete ct;
  return 0;
}
