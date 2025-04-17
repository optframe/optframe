module;

// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.component;

import optframe.semstream;

export import std;

#define OPTFRAME_CXX_MODULES 1

#include "OptFrame/Component.hpp"
