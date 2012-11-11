
#include "fake.h"

void
testfake() {

  string filename = "./testdata/assignee_comparison.csv";
  FakeTest * ft = new FakeTest("Fake testing...", filename);
  ft->load_fake_data(filename);
  delete ft;
}

int
main(int, char **) {

  testfake();
  return 0;
}
