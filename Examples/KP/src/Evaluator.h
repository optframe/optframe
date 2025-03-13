#ifndef KP_EVALUATOR_H_
#define KP_EVALUATOR_H_

// #include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>

#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

#define EPSILON_KP 0.0001

namespace KP {

// classic Evaluator is S dependent
class MyEvaluator : public Evaluator<SolutionKP, EvaluationKP, ESolutionKP> {
 private:
  ProblemInstance& pKP;

  // Your private vars

 public:
  MyEvaluator(ProblemInstance& _pKP);

  virtual ~MyEvaluator();

  Evaluation<> evaluate(const SolutionKP& s) override;

  // Evaluation<>& evaluate(const RepKP& rep, const MY_ADS&);

  virtual bool isMinimization() const override;
};

}  // namespace KP

#endif /*KP_EVALUATOR_H_*/
