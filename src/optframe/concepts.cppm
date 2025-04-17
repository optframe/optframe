module;

// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.concepts;
export import :my;
export import std;

#define OPTFRAME_CXX_MODULES 1

// #include <OptFrame/Concepts/BaseConcepts.hpp>
// -Winclude-angled-in-module-purview
#include "OptFrame/Concepts/BaseConcepts.hpp"
// #include <OptFrame/Concepts/Domain.hpp>
// -Winclude-angled-in-module-purview
#include "OptFrame/Concepts/Domain.hpp"