#pragma once

#include <algorithm>
#include <functional>
#include <iostream>

#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP1 = std::pair<
  std::vector<int>,  // first part of search space element: solution (representation)
  Evaluation<double> // second part of search space element: evaluation (objective value)
  >;

template<class X = nullptr_t>
using ESolutionTSP2 = ESolutionTSP1;

using ESolutionTSP = ESolutionTSP2<>;
