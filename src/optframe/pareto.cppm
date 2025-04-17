module;

#include <assert.h>
// #include <stdio.h>

// /usr/include/c++/15/type_traits:808:11: error: conflicting global module
// declaration
// #include <string>

export module optframe.pareto;
export import optframe.core;
export import optframe.concepts;

export import std;

// -Winclude-angled-in-module-purview
//
#define OPTFRAME_CXX_MODULES 1

#include "OptFrame/Pareto/MultiEvaluator.hpp"
//
#include "OptFrame/Pareto/MultiEvaluation.hpp"  // Maybe not Core... some Helper!
//
#include "OptFrame/Pareto/ParetoDominance.hpp"
#include "OptFrame/Pareto/ParetoDominanceWeak.hpp"
//
#include "OptFrame/Pareto/Pareto.hpp"
