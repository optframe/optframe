module;

#include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.nnptr;

export import std;

// #include <OptFrame/nnptr/nnshared.hpp>
// -Winclude-angled-in-module-purview
#include "OptFrame/nnptr/nnshared.hpp"
