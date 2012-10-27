

#include <engine.h> // typedef RecordList
#include <record.h>

vector<std::string>
get_column_names() {

  vector<std::string> cn;
  cn.push_back("Firstname");
  cn.push_back("Middlename");
  cn.push_back("Lastname");
  cn.push_back("Latitude");
  cn.push_back("Assignee");
  cn.push_back("City");
  cn.push_back("Country");
  cn.push_back("Patent");
  cn.push_back("ApplyYear");
  cn.push_back("Asgnum");
  return cn;
}


Record * make_foobar_record() {

  cFirstname  * firstname   = new cFirstname("Foo");
  cMiddlename * middlename  = new cMiddlename("X");
  cLastname   * lastname    = new cLastname("Bar");
  cLatitude   * latitude    = new cLatitude("42.00");
  cAssignee   * assignee    = new cAssignee("Gonesilent");
  cCity       * city        = new cCity("Burlingame");
  cCountry    * country     = new cCountry("US");
  cPatent     * patent      = new cPatent("07100123");
  cApplyYear  * applyyear   = new cApplyYear("2008");
  cAsgNum     * asgnum      = new cAsgNum("H000000064389");

  vector <const Attribute *> temp_vec_attrib;

  temp_vec_attrib.push_back(firstname);
  temp_vec_attrib.push_back(middlename);
  temp_vec_attrib.push_back(lastname);
  temp_vec_attrib.push_back(latitude);
  temp_vec_attrib.push_back(assignee);
  temp_vec_attrib.push_back(city);
  temp_vec_attrib.push_back(country);
  temp_vec_attrib.push_back(patent);
  temp_vec_attrib.push_back(applyyear);
  temp_vec_attrib.push_back(asgnum);

  Record * r = new Record(temp_vec_attrib);
  r->set_column_names(get_column_names());
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
  cApplyYear  * applyyear   = new cApplyYear("2008");
  cAsgNum     * asgnum      = new cAsgNum("H000000064389");

  vector <const Attribute *> temp_vec_attrib;

  temp_vec_attrib.push_back(firstname);
  temp_vec_attrib.push_back(middlename);
  temp_vec_attrib.push_back(lastname);
  temp_vec_attrib.push_back(latitude);
  temp_vec_attrib.push_back(assignee);
  temp_vec_attrib.push_back(city);
  temp_vec_attrib.push_back(country);
  temp_vec_attrib.push_back(patent);
  temp_vec_attrib.push_back(applyyear);
  temp_vec_attrib.push_back(asgnum);

  Record * r = new Record(temp_vec_attrib);
  r->set_column_names(get_column_names());
  return r;
}


RecordPList get_record_list() {
  Record * r1 = make_foobar_record();
  Record * r2 = make_quuxalot_record();
  RecordPList rl;
  rl.push_back(r1);
  rl.push_back(r2);
  return rl;
}


cBlocking_Operation_By_Coauthors get_blocker_coathor() {

  int num_coauthors_to_group = 4;
  RecordPList all_rec_pointers;
  cBlocking_Operation_By_Coauthors blocker_coauthor(all_rec_pointers, num_coauthors_to_group);
  return blocker_coauthor;
}


