#pragma once

#ifndef $project_REPRESENTATION_H_
#define $project_REPRESENTATION_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

#include "../../OptFrame/Util/Matrix.hpp"

#include "../../OptFrame/Solutions/CopySolution.hpp"

#include "../../OptFrame/Evaluation.hpp"

$include

using namespace std;

using namespace optframe;

// Solution Representation
using Rep$project = $rep;

using Solution$project = CopySolution<Rep$project>;

using Evaluation$project = Evaluation<double>;

// Remember that Rep$project must implement: (i) copy constructor and (ii) operator<<

// Auxiliary Data Structures (default 'int')
//using MY_ADS = int; // maybe try again in the future something like ADS$project...

// TODO Implement this
//#include <ostream>
//ostream & operator<< (ostream & s, Rep$project){}

#endif /*$project_REPRESENTATION_H_*/
