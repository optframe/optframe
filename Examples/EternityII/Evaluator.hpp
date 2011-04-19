#ifndef EtII_EVALUATOR_HPP_
#define EtII_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_EtII 0.0001

int numEvs = 0;

class EtIIEvaluator: public Evaluator<RepEtII, MemEtII>
{
private:
	EtIIProblemInstance& pEtII;

public:

	using Evaluator<RepEtII, MemEtII>::evaluate;

	EtIIEvaluator(EtIIProblemInstance& _pEtII) : // If necessary, add more parameters
		pEtII(_pEtII)
	{
		// Put the rest of your code here
	}

	EvaluationEtII& evaluate(const RepEtII& rep)
	{
		//counting evaluations.
		numEvs++;
		//cout<<"##### Number of evaluates: "<<numEvs<<endl;

		int fo = 0; // Evaluation Function Value

		// horizontal
		for (int i = 0; i < rep.getRows(); i++)
			for (int j = 0; j < rep.getCols() - 1; j++)
				if (rep(i, j).right == rep(i, j + 1).left)
					fo++;

		// vertical
		for (int j = 0; j < rep.getCols(); j++)
			for (int i = 0; i < rep.getRows() - 1; i++)
				if (rep(i, j).down == rep(i + 1, j).up)
					fo++;

		int& mem = *new int;
		mem = 0;

		return *new EvaluationEtII(fo, mem);
	}

	/*
	virtual void evaluate(Evaluation<MemEtII>& e, const RepEtII&)
	{
		e.setObjFunction(e.getObjFunction() + e.getM());
		e.getM() = 0;
	}
	*/

	virtual bool betterThan(double a, double b)
	{
		// MAXIMIZATION
		return (a > (b + EPSILON_EtII));
	}

};

#endif /*EtII_EVALUATOR_HPP_*/

