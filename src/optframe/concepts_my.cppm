module;

#include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.concepts:my;
//
export import optframe.nnptr;
export import optframe.opview;
export import std;

// #include <OptFrame/Concepts/MyConcepts.hpp>
// -Winclude-angled-in-module-purview
#include "OptFrame/Concepts/MyConcepts.hpp"
