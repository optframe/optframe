// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_CONSTRUCTIVE_RandomGreedy_H_
#define PN_CONSTRUCTIVE_RandomGreedy_H_

#include <OptFrame/Constructive.hpp>
//#include "../../OptFrame/Util/TestSolution.hpp"

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

class ConstructiveRandomGreedy
    : public Constructive<SolutionPN>  //<RepPN> //, MY_ADS>
{
 private:
  ProblemInstance& pPN;

  // Your private vars

 public:
  ConstructiveRandomGreedy(ProblemInstance& _pPN);

  virtual ~ConstructiveRandomGreedy();

  // Solution<ESolutionPN>* generateSolution(double timelimit);
  std::optional<SolutionPN> generateSolution(double timelimit) override;

  void print() const override { cout << "ConstructiveRandomGreedy" << endl; }
};

}  // namespace PN

#endif /*PN_CONTRUCTIVE_RandomGreedy_H_*/
