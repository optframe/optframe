// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_HPP
#define OPTFRAME_HEURISTICS_HPP

// Heuristics Module: inclusion of common heuristic strategies

// families
#include "OptFrame/Heuristics/EA/EA.hpp"
#include "OptFrame/Heuristics/EA/RK/RK.hpp"
#include "OptFrame/Heuristics/TS/TS.h"
#include "OptFrame/Heuristics/VNS/VNS.h"

// LocalSearches
#include "OptFrame/Heuristics/LocalSearches/Empty.hpp"
//
#include "OptFrame/Heuristics/LocalSearches/BI.hpp"

// Global Search
#include "OptFrame/Heuristics/EmptySingleObjSearch.hpp"

// SA family
#include "OptFrame/Heuristics/SA/SA.hpp"
//
#include "OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp"
//
// ILS family
#include "OptFrame/Heuristics/ILS/ILS.hpp"
//

#include "OptFrame/Heuristics/ILS/IteratedLocalSearch.hpp"
//
#include "OptFrame/Heuristics/ILS/ILSLPerturbation.hpp"
#include "OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp"
//
// EA family
#include "OptFrame/Heuristics/EA/EA.hpp"
// RK family
#include "OptFrame/Heuristics/EA/RK/RK.hpp"
//
#include "OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp"
//
#include "OptFrame/Heuristics/EA/RK/RKGA.hpp"
//
#include "OptFrame/Heuristics/EA/RK/BRKGA.hpp"
//
// GRASP
//
#include "OptFrame/Heuristics/GRASP/GRASPFamily.h"
#include "OptFrame/Heuristics/GRASP/GRConstructive.hpp"
//
// MO
//
#include "OptFrame/Heuristics/EmptyMultiObjSearch.hpp"

#endif  // OPTFRAME_HEURISTICS_HPP