#ifndef KP_CONSTRUCTIVE_Random_H_
#define KP_CONSTRUCTIVE_Random_H_

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/InitialSearch.hpp>
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

// class ConstructiveRandom: public Constructive<SolutionKP>
class ConstructiveRandom : public InitialSearch<ESolutionKP> {
 private:
  ProblemInstance& pKP;
  GeneralEvaluator<ESolutionKP>& eval;

 public:
  ConstructiveRandom(ProblemInstance& _pKP,
                     GeneralEvaluator<ESolutionKP>& _eval);

  virtual ~ConstructiveRandom();

  // std::optional<SolutionKP> generateSolution(double timelimit) override;
  std::pair<std::optional<ESolutionKP>, SearchStatus> initialSearch(
      const StopCriteria<EvaluationKP>& stop) override;
};

}  // namespace KP

#endif /*KP_CONTRUCTIVE_Random_H_*/
