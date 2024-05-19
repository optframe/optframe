#pragma once

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
// must come first...
#include <OptFrame/printable/printable.hpp>
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP =
    std::pair<std::vector<int>,  // first part of search space element: solution
                                 // (representation)
              Evaluation<int>    // second part of search space element:
                                 // evaluation (objective value)
              >;
