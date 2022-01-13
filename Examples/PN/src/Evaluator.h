#ifndef PN_EVALUATOR_H_
#define PN_EVALUATOR_H_

//#include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>

#include "DeltaStructure.h"
#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#define EPSILON_PN 0.0001

namespace PN {

class PNEvaluator : public Evaluator<SolutionPN, EvaluationPN, ESolutionPN> //RepPN, EvaluationPN> //public Evaluator<ESolutionPN>
{
private:
   ProblemInstance& pPN;

public:
   long long evaluations;

public:
   PNEvaluator(ProblemInstance& _pPN);

   virtual ~PNEvaluator();

   EvaluationPN evaluate(const SolutionPN& s) override;

   //EvaluationPN& evaluate(const RepPN& rep, const MY_ADS&);

   virtual bool betterThan(const EvaluationPN& f1, const EvaluationPN& f2) override;

   virtual bool isMinimization() const override
   {
      return true;
   }

   std::string toString() const override
   {
      return "PNEvaluator";
   }

   void print() const
   {
      cout << "Evaluation<> function for the Partition for Numbers (PN)" << endl;
   }
};

}

#endif /*PN_EVALUATOR_H_*/
