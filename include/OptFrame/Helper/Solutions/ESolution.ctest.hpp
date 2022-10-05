#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "../BaseConcepts.ctest.hpp"
#include "ESolution.hpp"

#ifndef NDEBUG
namespace optframe
{

struct optframe_test_debug_testpareto_esolution_disable_runtime
{
TestXSol< ESolution<double, int, Evaluation<int>> > testSol;

//TestEv< ESolution<double, int, Evaluation<int>> > testEv; // ESolution is not a 'XEvaluation' type, although it is 'XSolution'

//TestXESol< ESolution<double, void, Evaluation<int>> > testSol; // TODO: accept 'void' or not?

TestXESol< ESolution<double, int, Evaluation<int>> > testESol;
};


} // namespace optframe

#endif // NDEBUG