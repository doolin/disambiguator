
#include <string>
#include <cppunit/Portability.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/extensions/TestFactory.h>
#include <cppunit/TestCase.h>

#include <comparators.h>

using std::string;

class Strcmp95Test : public CppUnit::TestCase {

public:
  Strcmp95Test(std::string name) : CppUnit::TestCase(name) {}

};


void test_strcmp95() {

  Strcmp95Test * st = new Strcmp95Test(std::string("initial test"));
  delete st;
}



int
main(int argc, char ** argv) {
  test_strcmp95();
  return 0;
}
