#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "BaseConcepts.ctest.hpp"
#include "Pareto.hpp"


namespace optframe
{

// TODO: move this simple IsPareto test to BaseConcepts.test.hpp (just leave real Pareto class test here)
template <XSolution S, XEvaluation XEv>
struct IsPareto
{
};


struct optframe_test_debug_testpareto_ispareto_disable_runtime
{
TestPopOrPareto<Solution<double>, Evaluation<int>, IsPareto<Solution<double>,Evaluation<int>> > testLocal;
TestPopOrPareto<Solution<double>, Evaluation<int>, Population<Solution<double>,Evaluation<int>> > testPop;
TestPopOrPareto<Solution<double>, Evaluation<int>, Pareto<Solution<double>,Evaluation<int>> > testPareto;
};


} // namespace optframe