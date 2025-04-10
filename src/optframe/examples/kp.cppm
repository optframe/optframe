module;

#include <assert.h>
// #include <math.h>  // for ::fabs
// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.examples.kp;
export import optframe.core;
export import optframe.search;
export import optframe.heuristics;
export import optframe.functional;
export import optframe.scannerpp;
export import optframe.core;
export import optframe.search;
export import optframe.heuristics;
export import optframe.scannerpp;
export import optframe.util;
import optframe.hyper;

export import std;

#include "OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp"
