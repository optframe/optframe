// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef TSP_NSENUMShift_HPP_
#define TSP_NSENUMShift_HPP_

// Framework includes

#include "OptFrame/Move.hpp"
//
#include <OptFrame/NSEnum.hpp>
#include <OptFrame/Util/NSAdapter/NSEnumVectorShift.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

typedef MoveVectorShift<ESolutionTSP> MoveShiftTSP;
typedef NSEnumVectorShift<ESolutionTSP> NSEnumShiftTSP;

#endif /*TSP_NSENUMShift_HPP_*/
