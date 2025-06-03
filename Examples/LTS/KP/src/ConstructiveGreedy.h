#ifndef KP_CONSTRUCTIVE_Greedy_H_
#define KP_CONSTRUCTIVE_Greedy_H_

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

namespace KP {

class ConstructiveGreedy : public Constructive<SolutionKP> {
 private:
  ProblemInstance& pKP;

  // Your private vars

 public:
  ConstructiveGreedy(ProblemInstance& _pKP);

  virtual ~ConstructiveGreedy();

  std::optional<SolutionKP> generateSolution(double timelimit) override;
};

}  // namespace KP

#endif /*KP_CONTRUCTIVE_Greedy_H_*/
