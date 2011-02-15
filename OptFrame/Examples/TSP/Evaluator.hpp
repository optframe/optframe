#ifndef TSP_EVALUATOR_HPP_
#define TSP_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../../OptFrame/Evaluation.hpp"
#include "../../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define TSP_EPSILON 0.0001

class TSPEvaluator: public Evaluator<RepTSP, MemTSP>
{
private:
	TSPProblemInstance* pI;

public:

	using Evaluator<RepTSP, MemTSP>::evaluate; // prevents name hiding

	TSPEvaluator(TSPProblemInstance* pI) // If necessary, add more parameters
	{
		this->pI = pI;
	}

	EvaluationTSP& evaluate(const RepTSP& r)
	{
		double fo = 0; // Evaluation Function Value

		for (int i = 0; i < pI->n - 1; i++)
		{
			int j = r.at(i);
			int z = r.at(i + 1);

			double val = (*pI->dist)(j, z);
			fo += val;
		}

		int k = r.at(pI->n - 1);
		int l = r.at(0);

		double val = (*pI->dist)(k, l);
		fo += val;

		double mem = 0;

		return *new Evaluation<MemTSP> (fo, mem);
	}

	void evaluate(Evaluation<MemTSP>& e, const RepTSP& r)
	{
		e.setObjFunction(e.getObjFunction() + e.getM());
		e.setM(0);
	}

	virtual bool betterThan(double a, double b)
	{
		return (a < (b - TSP_EPSILON));
	}

};

#endif /*TSP_EVALUATOR_HPP_*/
