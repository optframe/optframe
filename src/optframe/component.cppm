module;

// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.component;

import optframe.semstream;

export import std;

#include <OptFrame/Component.hpp>
