

#include <engine.h> // typedef RecordList
#include <record.h>

Record * make_foobar_record() {

  cFirstname  * firstname   = new cFirstname("Foo");
  cMiddlename * middlename  = new cMiddlename("X");
  cLastname   * lastname    = new cLastname("Bar");
  cLatitude   * latitude    = new cLatitude("42.00");
  cAssignee   * assignee    = new cAssignee("Gonesilent");
  cCity       * city        = new cCity("Burlingame");
  cCountry    * country     = new cCountry("US");
  cPatent     * patent      = new cPatent("07100123");

  vector <const Attribute *> temp_vec_attrib;

  temp_vec_attrib.push_back(firstname);
  temp_vec_attrib.push_back(middlename);
  temp_vec_attrib.push_back(lastname);
  temp_vec_attrib.push_back(latitude);
  temp_vec_attrib.push_back(assignee);
  temp_vec_attrib.push_back(city);
  temp_vec_attrib.push_back(country);
  temp_vec_attrib.push_back(patent);

  Record * r = new Record(temp_vec_attrib);
  return r;
}


Record * make_quuxalot_record() {

  cFirstname  * firstname   = new cFirstname("Quux");
  cMiddlename * middlename  = new cMiddlename("A");
  cLastname   * lastname    = new cLastname("Lot");
  cLatitude   * latitude    = new cLatitude("42.00");
  cAssignee   * assignee    = new cAssignee("Gonesilent");
  cCity       * city        = new cCity("Burlingame");
  cCountry    * country     = new cCountry("US");
  cPatent     * patent      = new cPatent("07100124");

  vector <const Attribute *> temp_vec_attrib;

  temp_vec_attrib.push_back(firstname);
  temp_vec_attrib.push_back(middlename);
  temp_vec_attrib.push_back(lastname);
  temp_vec_attrib.push_back(latitude);
  temp_vec_attrib.push_back(assignee);
  temp_vec_attrib.push_back(city);
  temp_vec_attrib.push_back(country);
  temp_vec_attrib.push_back(patent);

  Record * r = new Record(temp_vec_attrib);
  return r;
}


RecordList get_record_list() {
  Record * r1 = make_foobar_record();
  Record * r2 = make_quuxalot_record();
  RecordList rl;
  rl.push_back(r1);
  rl.push_back(r2);
  return rl;
}


cBlocking_Operation_By_Coauthors get_blocker_coathor() {

  int num_coauthors_to_group = 4;
  RecordList all_rec_pointers;
  cBlocking_Operation_By_Coauthors blocker_coauthor(all_rec_pointers, num_coauthors_to_group);
  return blocker_coauthor;
}


