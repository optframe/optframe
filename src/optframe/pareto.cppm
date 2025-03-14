module;

#include <assert.h>
#include <stdio.h>

#include <string>

export module optframe.pareto;
export import optframe.core;
export import optframe.concepts;

export import std;

//
#include <OptFrame/Pareto/MultiEvaluator.hpp>
//
#include <OptFrame/Pareto/MultiEvaluation.hpp>  // Maybe not Core... some Helper!
//
#include <OptFrame/Pareto/ParetoDominance.hpp>
#include <OptFrame/Pareto/ParetoDominanceWeak.hpp>
//
#include <OptFrame/Pareto/Pareto.hpp>
