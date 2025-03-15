module;

#include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.scannerpp;

export import std;

// #include <OptFrame/Scanner++/Scanner.hpp>
// // -Winclude-angled-in-module-purview
#include "OptFrame/Scanner++/Scanner.hpp"