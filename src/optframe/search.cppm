module;

#include <assert.h>
#include <stdio.h>

//  /usr/include/c++/15/type_traits:808:11: error: conflicting global module
//  declaration
//  #include <string>

export module optframe.search;
export import optframe.component;
export import optframe.core;
export import optframe.pareto;
export import optframe.concepts;  // testing Empty!

export import std;

// // -Winclude-angled-in-module-purview
#include "OptFrame/Search.hpp"
