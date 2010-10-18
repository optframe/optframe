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


class TSPEvaluator: public Evaluator<RepTSP, MemTSP>
{
private:
	TSPProblemInstance* pI;

public:
	TSPEvaluator(TSPProblemInstance* pI) // If necessary, add more parameters
	{
		this->pI = pI;
	}

	EvaluationTSP* evaluate(SolutionTSP* s)
	{
		double fo = 0; // Evaluation Function Value

		vector<double>* reval = new vector<double>(pI->n);

		for(int i=0;i< pI->n -1;i++)
		{
			int j = s->getRepresentation()->at(i);
			int z = s->getRepresentation()->at(i+1);

			double val = (*pI->dist)(j,z);
			(*reval)[i] = val;
			fo += val;
		}

		int k = s->getRepresentation()->at(pI->n-1);
		int l = s->getRepresentation()->at(0);

		double val = (*pI->dist)(k,l);
		(*reval)[pI->n-1] = val;
		fo += val;

		return new Evaluation<MemTSP>(fo,reval);
	}

};

#endif /*TSP_EVALUATOR_HPP_*/
