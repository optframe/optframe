#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "Population.hpp"
#include "Pareto.hpp"


namespace optframe
{

// ======= begin pareto and population tests

template <XSolution S, XEvaluation XEv>
struct IsPopOrPareto
{
};

//template<template<XSolution S, XEvaluation XEv> X2ESolution<S, XEv> X2ES> // issue with concept template template
template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class TestPopOrPareto
{
public:
};

struct optframe_test_debug_testpareto_ispoporpareto_disable_runtime
{
TestPopOrPareto<Solution<double>, Evaluation<int>, IsPopOrPareto<Solution<double>,Evaluation<int>> > testLocal;
TestPopOrPareto<Solution<double>, Evaluation<int>, Population<Solution<double>,Evaluation<int>> > testPop;
TestPopOrPareto<Solution<double>, Evaluation<int>, Pareto<Solution<double>,Evaluation<int>> > testPareto;
};


} // namespace optframe