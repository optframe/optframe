// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Iterated Local Search

// C++
#include <cstdint>
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/ILS/ILSL.hpp>
#include <OptFrame/Heuristics/LocalSearches/BI.hpp>
#include <OptFrame/Heuristics/LocalSearches/VND.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;
