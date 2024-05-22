// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_PN_INCLUDE_PN_PROBLEMCOMMAND_H_
#define EXAMPLES_PN_INCLUDE_PN_PROBLEMCOMMAND_H_

// C++
#include <iostream>
#include <string>

//

// #include "../../OptFrame/Commands/ProblemCommand.hpp"
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

#include "ConstructiveGreedyHalfSum.h"
#include "ConstructiveGreedySum.h"
#include "ConstructiveRandom.h"
#include "ConstructiveRandomGreedy.h"
#include "DeltaStructure.h"
#include "Evaluation.h"
#include "Evaluator.h"
#include "NSSeqShift.h"
#include "NSSeqSwap.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace scannerpp;
using namespace optframe;

namespace PN {

class PNProblemCommand {
 public:
  ProblemInstance* p;

  PNProblemCommand() { p = NULL; }

  virtual ~PNProblemCommand() {}

  string id() { return "problem.PN"; }

  vsref<Component> vd;

  void registerComponent(sref<Component> c, string s) { vd.push_back(c); }

  // Implement 'load' and 'unload' methods in .cpp file

  bool load(string filename, HeuristicFactory<ESolutionPN>& hf,
            map<string, string>& dictionary,
            map<string, vector<string>>& ldictionary);

  bool unload(HeuristicFactory<ESolutionPN>& factory,
              map<string, string>& dictionary,
              map<string, vector<string>>& ldictionary);
};

}  // namespace PN

#endif  // EXAMPLES_PN_INCLUDE_PN_PROBLEMCOMMAND_H_
