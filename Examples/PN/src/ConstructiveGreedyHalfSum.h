#ifndef PN_CONSTRUCTIVE_GreedyHalfSum_H_
#define PN_CONSTRUCTIVE_GreedyHalfSum_H_

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

#endif /*PN_CONTRUCTIVE_GreedyHalfSum_H_*/
