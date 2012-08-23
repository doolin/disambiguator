
#include <record.h>

  void make_foobar_record() {

    cFirstname  * firstname   = new cFirstname("Foo");
    cMiddlename * middlename  = new cMiddlename("X");
    cLastname   * lastname    = new cLastname("Bar");
    cLatitude   * latitude    = new cLatitude("42.00");
    cAssignee   * assignee    = new cAssignee("Gonesilent");
    cCity       * city        = new cCity("Burlingame");
    cCountry    * country     = new cCountry("US");

    vector <const Attribute *> temp_vec_attrib;

    temp_vec_attrib.push_back(firstname);
    temp_vec_attrib.push_back(middlename);
    temp_vec_attrib.push_back(lastname);
    temp_vec_attrib.push_back(latitude);
    temp_vec_attrib.push_back(assignee);
    temp_vec_attrib.push_back(city);
    temp_vec_attrib.push_back(country);

    Record tmprec(temp_vec_attrib);
    std::cout << "Record..." << std::endl;
  }


  void make_quuxalot_record() {

    cFirstname  * firstname   = new cFirstname("Quux");
    cMiddlename * middlename  = new cMiddlename("A");
    cLastname   * lastname    = new cLastname("Lot");
    cLatitude   * latitude    = new cLatitude("42.00");
    cAssignee   * assignee    = new cAssignee("Gonesilent");
    cCity       * city        = new cCity("Burlingame");
    cCountry    * country     = new cCountry("US");

    vector <const Attribute *> temp_vec_attrib;

    temp_vec_attrib.push_back(firstname);
    temp_vec_attrib.push_back(middlename);
    temp_vec_attrib.push_back(lastname);
    temp_vec_attrib.push_back(latitude);
    temp_vec_attrib.push_back(assignee);
    temp_vec_attrib.push_back(city);
    temp_vec_attrib.push_back(country);

    Record tmprec(temp_vec_attrib);
    std::cout << "Record..." << std::endl;
  }


