#pragma once

// C++
#include <utility>
//
#include <OptFrame/Evaluation.hpp>       // JUST FOR TESTING!
#include <OptFrame/Helper/Solution.hpp>  // JUST FOR TESTING!
#include <OptFrame/VEPopulation.hpp>

namespace optframe {

// compilation tests
#ifndef NDEBUG

static_assert(X2ESolution<
              VEPopulation<std::pair<Solution<double>, Evaluation<double>>>,
              std::pair<Solution<double>, Evaluation<double>>>);

#endif

}  // namespace optframe
