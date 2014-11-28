#ifndef PN_EVALUATOR_H_
#define PN_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#define EPSILON_PN 0.0001


namespace PN
{

class PNEvaluator: public Evaluator<RepPN, MY_ADS >
{
private:
	ProblemInstance& pPN;
public:
	long long evaluations;

public:
	PNEvaluator(ProblemInstance& _pPN);
	
	virtual ~PNEvaluator();

	EvaluationPN& evaluate(const RepPN& rep);
	
	//EvaluationPN& evaluate(const RepPN& rep, const MY_ADS&);

	virtual bool betterThan(double f1, double f2);

	virtual bool isMinimization() const
	{
		return true;
	}

	void print() const
	{
		cout << "Evaluation function for the Partition for Numbers (PN)" << endl;
	}
};

}

#endif /*PN_EVALUATOR_H_*/

