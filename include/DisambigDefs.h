/*
 * DisambigDefs.h
 *
 *  Created on: Dec 7, 2010
 *  Author: Ye (Edward) Sun.  Harvard University. sunyedinken2002@hotmail.com
 *
 *  This file contains most of the basic definitions.
 */


#ifndef DISAMBIGLIB_HPP_
#define DISAMBIGLIB_HPP_

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <pthread.h>
#include <typeinfo>
#include <memory>

#define INERT_ATTRIB_GROUP_IDENTIFIER "NONE" // the attribute group specifier that is not the component of similarity profiles


using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

class cRecord;
class cRecord_Reconfigurator;
class Attribute;

void cRecord_update_active_similarity_names();
const cRecord_Reconfigurator * generate_interactive_reconfigurator( const Attribute * pAttrib);
void reconfigure_interactives ( const cRecord_Reconfigurator * pc, const cRecord * pRec);


#include "exceptions.h"
#include "attribute.h"

#endif /* DISAMBIGLIB_HPP_ */
