#pragma once

#ifndef $project_REPRESENTATION_H_
#define $project_REPRESENTATION_H_

#include <vector>
#include <OptFrame/Util/printable.h>

#include <OptFrame/Util/Matrix.hpp>

#include <OptFrame/Solutions/CopySolution.hpp>

#include <OptFrame/Evaluation.hpp>

$include

using namespace std;

using namespace optframe;

// Solution Representation
using Rep$project = $rep;

// Using Copy-based solution (requires copy constructor on R and empty constructor for optional ADS)
// If you prefer a Pointer-based solution, you can use class Solution<Rep$project>
using Solution$project = CopySolution<Rep$project>;

// Default objective value type is 'double'
using Evaluation$project = Evaluation<double>;

typedef pair< Solution$project , Evaluation$project > ESolution$project ;

// compilation tests
static_assert(XSolution< Solution$project >); // verify that this is correctly a XSolution
static_assert(XESolution< ESolution$project >); // verify that this is correctly a XESolution
static_assert(XSearch< ESolution$project , ESolution$project >); // verify that this is correctly a XSearch


// Remember that Rep$project must implement: (i) copy constructor and (ii) operator<<

// Auxiliary Data Structures (default 'int')
//using MY_ADS = int; // maybe try again in the future something like ADS$project...

// TODO Implement this
//#include <ostream>
//ostream & operator<< (ostream & s, Rep$project){}

#endif /*$project_REPRESENTATION_H_*/
