#ifndef PN_PROBLEMMODULE_H_
#define PN_PROBLEMMODULE_H_

#include <iostream>

//#include "../../OptFrame/Commands/ProblemCommand.hpp"
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

class PNProblemCommand  //: public ProblemCommand<RepPN , MY_ADS >
{
 public:
  ProblemInstance* p;

  PNProblemCommand() {
    p = NULL;
  }

  virtual ~PNProblemCommand() {
  }

  string id() {
    return "problem.PN";
  }

  vsref<Component> vd;

  void registerComponent(sref<Component> c, string s) {
    vd.push_back(c);
  }

  // Implement 'load' and 'unload' methods in .cpp file

  bool load(string filename, HeuristicFactory<SolutionPN, EvaluationPN, ESolutionPN>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary);

  bool unload(HeuristicFactory<SolutionPN, EvaluationPN, ESolutionPN>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary);
};

}  // namespace PN

#endif /*PN_PROBLEMMODULE_H_*/
