// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_HPP
#define OPTFRAME_HEURISTICS_HPP

// Heuristics Module: inclusion of common heuristic strategies

// LocalSearches
#include "Heuristics/LocalSearches/Empty.hpp"
//
#include "Heuristics/LocalSearches/BestImprovement.hpp"

// Global Search
#include "Heuristics/EmptySingleObjSearch.hpp"

// SA family
#include "Heuristics/SA/SA.hpp"
//
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"
//
// ILS family
#include "Heuristics/ILS/ILS.hpp"
//

#include "Heuristics/ILS/IteratedLocalSearch.hpp"
//
#include "Heuristics/ILS/ILSLPerturbation.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
//
// EA family
#include "Heuristics/EA/EA.hpp"
// RK family
#include "Heuristics/EA/RK/RK.hpp"
//
#include "Heuristics/EA/RK/DecoderRandomKeys.hpp"
//
#include "Heuristics/EA/RK/RKGA.hpp"
//
#include "Heuristics/EA/RK/BRKGA.hpp"
//
// GRASP
//
#include "Heuristics/GRASP/GRASPFamily.h"
#include "Heuristics/GRASP/GRConstructive.hpp"
//
// MO
//
#include "Heuristics/EmptyMultiObjSearch.hpp"

#endif  // OPTFRAME_HEURISTICS_HPP