#ifndef OPM_REPRESENTATION_H_
#define OPM_REPRESENTATION_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"
#include "../../OptFrame/Util/Matrix.hpp"

using namespace std;

// Solution Representation
typedef pair < vector < int > ,Matrix < int >   >  RepOPM;

//#include <ostream>
//ostream & operator<< (ostream & s, RepOPM){}

#endif /*OPM_REPRESENTATION_H_*/

