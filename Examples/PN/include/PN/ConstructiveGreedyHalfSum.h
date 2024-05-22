// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_PN_INCLUDE_PN_CONSTRUCTIVEGREEDYHALFSUM_H_
#define EXAMPLES_PN_INCLUDE_PN_CONSTRUCTIVEGREEDYHALFSUM_H_

// C
#include <stdlib.h>

// C++
#include <algorithm>
#include <list>
//
#include <OptFrame/Constructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;

using namespace optframe;

namespace PN {

class ConstructiveGreedyHalfSum
    : public Constructive<SolutionPN>  //<RepPN> //, MY_ADS>
{
 private:
  ProblemInstance& pPN;

  // Your private vars

 public:
  ConstructiveGreedyHalfSum(ProblemInstance& _pPN);

  virtual ~ConstructiveGreedyHalfSum();

  // Solution<ESolutionPN>* generateSolution(double timelimit);
  std::optional<SolutionPN> generateSolution(double timelimit) override;

  void print() const override { cout << "ConstructiveGreedyHalfSum" << endl; }
};

}  // namespace PN

#endif  // EXAMPLES_PN_INCLUDE_PN_CONSTRUCTIVEGREEDYHALFSUM_H_
