
#include "fake.h"

void
testfake() {

  string filename = "./testdata/assignee_comparison.csv";
  string title = "Fake testing...";
  FakeTest * ft = new FakeTest(title, filename);
  ft->load_fake_data(filename);
  delete ft;
}

int
main(int, char **) {

  testfake();
  return 0;
}
