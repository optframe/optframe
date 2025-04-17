module;

#include <assert.h>
// #include <math.h>  // for ::fabs
// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.functional;
export import optframe.core;
export import optframe.search;
export import optframe.heuristics;
export import optframe.hyper;

export import std;

#define OPTFRAME_CXX_MODULES 1

#include "OptFCore/FCoreAll.hpp"
