/*
 * DisambigEngine.h
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
 */

#ifndef DISAMBIGENGINE_H_
#define DISAMBIGENGINE_H_

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "DisambigDefs.h"
#include "Threading.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

//asgdetail consists of assignee number and its patent counts.
typedef std::pair<string, unsigned int> asgdetail;

// cGroup_Value is a list of const cRecord pointers. This definition will be used throughout the whole project.
typedef list<const cRecord*> cGroup_Value;

// forward diclarations
class cBlocking_Operation_By_Coauthors;

#include "record.h"
#include "engine.h"

#endif /* DISAMBIGENGINE_H_ */
