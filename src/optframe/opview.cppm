module;

#include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.opview;

export import std;

#include <OptFrame/opview/optional_view.hpp>
