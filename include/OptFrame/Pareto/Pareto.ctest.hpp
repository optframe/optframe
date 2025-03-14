#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include <OptFrame/Concepts/BaseConcepts.ctest.hpp>

#include "Pareto.hpp"

namespace optframe {

using EMSolution_Test = std::pair<IsSolution, MultiEvaluation<double>>;

struct optframe_test_debug_testpareto_ispareto_disable_runtime {
  TestPopOrPareto<EMSolution_Test, Pareto<EMSolution_Test>> testPareto;
};

}  // namespace optframe