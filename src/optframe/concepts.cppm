module;

// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.concepts;
export import :my;
export import std;

#include <OptFrame/Concepts/BaseConcepts.hpp>
// TODO: move Domain to concepts
#include <OptFrame/Concepts/Domain.hpp>
