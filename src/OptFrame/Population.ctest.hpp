#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "BaseConcepts.ctest.hpp"
#include "Population.hpp"

#ifndef NDEBUG
namespace optframe
{

// TODO: move this simple IsPopulation test to BaseConcepts.ctest.hpp (just leave real Population class test here)
template <XSolution S, XEvaluation XEv>
struct IsPopulation
{
};


struct optframe_test_debug_testpareto_ispopulation_disable_runtime
{
TestPopOrPareto<Solution<double>, Evaluation<int>, IsPopulation<Solution<double>,Evaluation<int>> > testLocal; // todo: move to BaseConcepts.ctest.hpp
TestPopOrPareto<Solution<double>, Evaluation<int>, Population<Solution<double>,Evaluation<int>> > testPop;
};


} // namespace optframe

#endif // NDEBUG