#include <cstdio>
#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <training.h>
#include <engine.h>
#include <ratios.h>
#include <record.h>

#include "colortest.h"
#include "testutils.h"
#include "fake.h"

class TrainingTest : public CppUnit::TestCase {

private:
  FakeTest * ft;
  RecordPList recpointers;
  vector<const Record*> rpv;

public:

  TrainingTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());

    const string filename("testdata/assignee_comparison.csv");
    ft = new FakeTest(string("Fake training test"), filename);
    ft->load_fake_data(filename);
    recpointers = ft->get_recpointers();
    rpv = ft->get_recvecs();
  }


 ~TrainingTest() {
   describe_test(INDENT2, "Destroying TrainingTest");
   delete ft;
 }


  void test_get_blocking_indice() {

    describe_test(INDENT2, "Testing get_blocking_indice()");

    Spec spec;

    spec.it("Blocking index for Firstname/Lastname/Class/Coauthor: (0,1,13,12)", DO_SPEC {
      vector<string> column_names = {
        "Firstname", "Lastname", "Class", "Coauthor"
      };
      vector<uint32_t> bi = get_blocking_indices(column_names);
      // The order of the blocking indices depends on how the column
      // names are ordered when the data is read from the configuration file.
      // We're controlling the order in the FakeTest constructor here.
      vector<uint32_t> target = { 0, 1, 13, 12 };
      return (target == bi);
    });

  }


  void runTest() {
    test_get_blocking_indice();
  }
};


void
test_training() {

  TrainingTest * tt = new TrainingTest(string("Training initial test"));
  tt->runTest();
  delete tt;
}

#ifdef test_training_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_training();
  return 0;
}
#endif
