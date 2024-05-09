// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef HFMVRP_H_
#define HFMVRP_H_

#include "ADS.h"
#include "DeltaStructure.h"
#include "Evaluation.h"
#include "Evaluator.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"
//
// #include "Neighborhoods/Inter/DeltaMoveVRPCross.hpp"
#include "Neighborhoods/Inter/DeltaIteratorVRPShift10.hpp"
#include "Neighborhoods/Inter/DeltaIteratorVRPSwap1_1.hpp"
#include "Neighborhoods/Inter/DeltaMoveVRPShift10.hpp"
#include "Neighborhoods/Inter/DeltaMoveVRPSwap1_1.hpp"
#include "Neighborhoods/Intra/DeltaIteratorVRP2Opt.hpp"
#include "Neighborhoods/Intra/DeltaIteratorVRPExchange.hpp"
#include "Neighborhoods/Intra/DeltaIteratorVRPOrOpt1.hpp"
#include "Neighborhoods/Intra/DeltaIteratorVRPOrOpt2.hpp"
#include "Neighborhoods/Intra/DeltaMoveVRP2Opt.hpp"
#include "Neighborhoods/Intra/DeltaMoveVRPExchange.hpp"
#include "Neighborhoods/Intra/DeltaMoveVRPOrOpt1.hpp"
#include "Neighborhoods/Intra/DeltaMoveVRPOrOpt2.hpp"
//
#include "ADSManager.hpp"
#include "ConstructiveSavings.hpp"
#include "ConstructiveSavingsRandomized.hpp"
#endif /*HFMVRP_H_*/
