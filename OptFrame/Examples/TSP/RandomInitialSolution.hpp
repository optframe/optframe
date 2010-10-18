#ifndef TSP_INITIALSOLUTION_Random_HPP_
#define TSP_INITIALSOLUTION_Random_HPP_

#include "../../../OptFrame/InitialSolution.h"
#include "../../../OptFrame/Solution/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

class RandomInitialSolutionTSP: public InitialSolution<RepTSP>
{
private:
	TSPProblemInstance* pI;

	// Your private vars

public:

	RandomInitialSolutionTSP(TSPProblemInstance* pI) // If necessary, add more parameters
	{
		this->pI = pI;

		// Put the rest of your code here
	};

	SolutionTSP* generateSolution()
	{
		RepTSP* newRep = new vector<int>(pI->n);

		vector<int>* r = new vector<int>(pI->n);
		for(unsigned int i=0;i<r->size();i++)
			(*r)[i]=i;

		//std::random_shuffle( r->begin(), r->end() ); // shuffle elements of r

		for(unsigned int i=0;i<newRep->size();i++)
			(*newRep)[i] = (*r)[i];

		delete r;

		return new TestSolution<RepTSP>(newRep);
	}

};

#endif /*TSP_INITIALSOLUTION_Random_HPP_*/
