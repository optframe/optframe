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

class PNEvaluator : public Evaluator<RepPN, EvaluationPN> //public Evaluator<RepPN, MY_ADS>
{
private:
   ProblemInstance& pPN;

public:
   long long evaluations;

public:
   PNEvaluator(ProblemInstance& _pPN);

   virtual ~PNEvaluator();

   EvaluationPN evaluate(const RepPN& rep, const OPTFRAME_DEFAULT_ADS*);

   //EvaluationPN& evaluate(const RepPN& rep, const MY_ADS&);

   virtual bool betterThan(double f1, double f2);

   virtual bool isMinimization() const
   {
      return true;
   }

   void print() const
   {
      cout << "Evaluation<> function for the Partition for Numbers (PN)" << endl;
   }
};

}

#endif /*PN_EVALUATOR_H_*/
