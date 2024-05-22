// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_SOLUTION_H_
#define PN_SOLUTION_H_

#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "Evaluation.h"
#include "Representation.h"

using namespace optframe;

typedef int MY_ADS;

typedef CopySolution<RepPN, MY_ADS> SolutionPN;

using ESolutionPN = std::pair<SolutionPN, EvaluationPN>;

#endif /*PN_SOLUTION_H_*/
