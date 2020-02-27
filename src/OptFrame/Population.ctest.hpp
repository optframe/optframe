#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "BaseConcepts.ctest.hpp"
#include "Population.hpp"

#ifndef NDEBUG
namespace optframe
{

struct optframe_test_debug_testpareto_ispopulation_disable_runtime
{
TestPopOrPareto<Solution<double>, Evaluation<int>, Population<Solution<double>,Evaluation<int>> > testPop;
};


} // namespace optframe

#endif // NDEBUG