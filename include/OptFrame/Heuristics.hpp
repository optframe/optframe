// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_HPP
#define OPTFRAME_HEURISTICS_HPP

// Heuristics Module: inclusion of common heuristic strategies

// families
#include "OptFrame/Heuristics/EA/FamilyEA.hpp"
#include "OptFrame/Heuristics/EA/RK/FamilyRK.hpp"
#include "OptFrame/Heuristics/TS/FamilyTS.h"
#include "OptFrame/Heuristics/VNS/FamilyVNS.h"

// LocalSearches
#include "OptFrame/Heuristics/LocalSearches/EmptyLS.hpp"
//
#include "OptFrame/Heuristics/LocalSearches/BI.hpp"

// Global Search
#include "OptFrame/Heuristics/EmptySingleObjSearch.hpp"

// SA family
#include "OptFrame/Heuristics/SA/FamilySA.hpp"
//
#include "OptFrame/Heuristics/SA/BasicSA.hpp"
//
// ILS family
#include "OptFrame/Heuristics/ILS/FamilyILS.hpp"
//

#include "OptFrame/Heuristics/ILS/MetaILS.hpp"
//
#include "OptFrame/Heuristics/ILS/ILSL.hpp"
#include "OptFrame/Heuristics/ILS/ILSLPerturbation.hpp"
//
// EA family
#include "OptFrame/Heuristics/EA/FamilyEA.hpp"
// RK family
#include "OptFrame/Heuristics/EA/RK/FamilyRK.hpp"
//
#include "OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp"
//
#include "OptFrame/Heuristics/EA/RK/RKGA.hpp"
//
#include "OptFrame/Heuristics/EA/RK/BRKGA.hpp"
//
// GRASP
//
#include "OptFrame/Heuristics/GRASP/FamilyGRASP.h"
#include "OptFrame/Heuristics/GRASP/GRConstructive.hpp"
//
// MO
//
#include "OptFrame/Heuristics/EmptyMultiObjSearch.hpp"

#endif  // OPTFRAME_HEURISTICS_HPP