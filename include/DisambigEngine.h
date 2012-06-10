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

#include "attribute.h"
#include "Threading.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

// forward declarations
//class cBlocking_Operation_By_Coauthors;


// cGroup_Value is a list of const Record pointers.
// This definition will be used throughout the whole project.
// TODO: This is ludicrous that Record can be used in a typedef
// before it's defined in its header file below. This means
// there is some header include somewhere which is doing
// the include as a side effect.
typedef std::list<const Record * > cGroup_Value;

#include "record.h"
#include "engine.h"


#endif /* DISAMBIGENGINE_H_ */
