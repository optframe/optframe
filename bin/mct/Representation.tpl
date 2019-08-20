#ifndef $project_REPRESENTATION_H_
#define $project_REPRESENTATION_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

#include "../../OptFrame/Util/Matrix.hpp"

#include "../../OptFrame/Solutions/CopySolution.hpp"

$include

using namespace std;

using namespace optframe;

// Solution Representation
typedef $rep Rep$project;

typedef CopySolution<Rep$project> Solution$project;

// Remember that Rep$project must implement: (i) copy constructor and (ii) operator<<

// Auxiliary Data Structures (default 'int')
//typedef int MY_ADS; // maybe try again in the future something like ADS$project...

//#include <ostream>
//ostream & operator<< (ostream & s, Rep$project){}

#endif /*$project_REPRESENTATION_H_*/

