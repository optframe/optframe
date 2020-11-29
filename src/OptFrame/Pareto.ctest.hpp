#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "BaseConcepts.ctest.hpp"
#include "Pareto.hpp"


namespace optframe
{

struct optframe_test_debug_testpareto_ispareto_disable_runtime
{
   TestPopOrPareto<ESolution<double>, Pareto<ESolution<double>> > testPareto;
};


} // namespace optframe