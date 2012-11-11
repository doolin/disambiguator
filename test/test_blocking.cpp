/** @file */
#include <string>
#include <vector>

#include <cppunit/TestCase.h>

#include <disambiguation.h>
#include <engine.h>
#include <cluster.h>
#include <clusterinfo.h>
#include <training.h>
#include <record.h>
#include <ratios.h>

#include "testdata.h"
#include "testutils.h"
#include "fake.h"


#include <disambiguate.h>
#include <attribute.h>
#include <cluster.h>
#include <newcluster.h>
#include <record.h>
#include <engine.h>
#include <blocking_operation.h>
#include "fake.h"

#include "testutils.h"


class BlockingTest : public CppUnit::TestCase {

private:
  FakeTest * ft;
  RecordPList recpointers;
  vector<const Record*> rpv;
  const cBlocking_Operation_By_Coauthors * cob;

public:
  BlockingTest(std::string name) : CppUnit::TestCase(name) {

    describe_test(INDENT0, name.c_str());
    // No middle names in clustertest.csv
    //const string filename("testdata/clustertest.csv");
    // assignee_comparison has middle names
    const string filename("testdata/assignee_comparison.csv");
    ft = new FakeTest(string("Fake RatioComponentTest"), filename);
    ft->load_fake_data(filename);
    recpointers = ft->get_recpointers();
    rpv = ft->get_recvecs();
    cob = ft->get_coauthor_blocking();
   }


  void test_read_blocking_config() {

    Spec spec;

    spec.it("Reads a valid blocking file correctly", DO_SPEC {
       std::ofstream devnull("/dev/null");
       int result;
       result = BlockingConfiguration::config_blocking("./fixtures/BlockingConfig.txt", string("Round 1"), devnull);
       return (0 == result);
    });

    // TODO: handle result = 1
    // TODO: handle result = 2
  }


  void test_multi_column_blocking() {

    describe_test(INDENT2, "Testing multicolumn blocking");

    Spec spec;

    std::auto_ptr<cBlocking_Operation> bptr = get_blocking_pointer();
    const BlockByColumns & blocker_ref = dynamic_cast<BlockByColumns &> (*bptr);
    vector<string> names = blocker_ref.get_blocking_attribute_names();

    StringRemoveSpace rsobj;
    StringNoSpaceTruncate nstobj;
    nstobj.set_truncater(2, 4, true);
    string man = nstobj.manipulate("EXAMPLES");

    spec.it("Manipulate EXAMPLE to acquire AMPL", [&man](Description desc)->bool {
        return (man == string("AMPL"));
    });

    vector<const StringManipulator *> vec_strman = {
      &rsobj, &rsobj, &nstobj
    };

    vector<string> vec_label = { "Firstname", "Middlename", "Lastname" };
    vector<uint32_t> vec_di = { 1, 0, 1 };
    BlockByColumns mcmobj (vec_strman, vec_label, vec_di);

    const Record * r = rpv[0];

    spec.it("Does some blocking on a first/middle/last: %s", [r,&mcmobj,&spec](Description desc)->bool {
       string binfo = mcmobj.extract_blocking_info(r);
       sprintf(spec.buf, desc, binfo.c_str());
       return (string("PHILIP##E##RAND##") == binfo);
    });

    spec.it("Does some blocking on a first/middle/last: %s", [r,&mcmobj,&spec](Description desc)->bool {
       string cinfo = mcmobj.extract_column_info(r,2);
       sprintf(spec.buf, desc, cinfo.c_str());
       return (string("RAND") == cinfo);
    });

  }


  void test_get_topN_coauthors() {
    const Record * prec = recpointers.front();
    std::cout << "front pointer: " << recpointers.front() << std::endl;
    prec->print();
    //cob->get_topN_coauthors(prec, 2);
  }


  void test_coauthor_blocking() {
    //const Record * prec = recpointers.front();
    std::cout << "front pointer: " << recpointers.front() << std::endl;
    //std::string bi = cob->extract_blocking_info(recpointers.front());
    //std::cout << "blocking info: " << bi << std::endl;
  }


  void runTest() {
    test_read_blocking_config();
    test_multi_column_blocking();
    //test_get_topN_coauthors();
    //test_coauthor_blocking();
  }
};


void
test_blocking() {

  BlockingTest * bt = new BlockingTest(std::string("Blocking test..."));
  bt->runTest();
  delete bt;
}



#ifdef test_blocking_STANDALONE
int
main(int UP(argc), char ** UP(argv)) {

  test_blocking();
  return 0;
}
#endif
