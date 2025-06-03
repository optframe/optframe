// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_CONSTRUCTIVE_GreedySum_H_
#define PN_CONSTRUCTIVE_GreedySum_H_

#include <OptFrame/Core/Constructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"

#include <stdlib.h>

#include <algorithm>
#include <list>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;

using namespace optframe;

namespace PN {

class ConstructiveGreedySum
    : public Constructive<SolutionPN>  //<RepPN> //, MY_ADS>
{
 private:
  ProblemInstance& pPN;

  // Your private vars

 public:
  ConstructiveGreedySum(ProblemInstance& _pPN);

  virtual ~ConstructiveGreedySum();

  // Solution<ESolutionPN>* generateSolution(double timelimit);
  std::optional<SolutionPN> generateSolution(double timelimit) override;

  void print() const override { std::cout << "ConstructiveGreedySum" << std::endl; }
};

}  // namespace PN

#endif /*PN_CONTRUCTIVE_GreedySum_H_*/
