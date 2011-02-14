#ifndef TSP_INITIALSOLUTION_Random_HPP_
#define TSP_INITIALSOLUTION_Random_HPP_

#include "../../../OptFrame/InitialSolution.h"
#include "../../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"
#include "../../RandGen.hpp"
#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

class RandomInitialSolutionTSP: public InitialSolution<RepTSP>
{
private:
	TSPProblemInstance* pI;
	RandGen& rg;

	// Your private vars

public:

	RandomInitialSolutionTSP(TSPProblemInstance* pI,RandGen& _rg): rg(_rg) // If necessary, add more parameters
	{
		this->pI = pI;
		// Put the rest of your code here
	};

	SolutionTSP& generateSolution()
	{
		RepTSP* newRep = new vector<int>(pI->n);

		vector<int>* r = new vector<int>(pI->n);
		for(unsigned int i=0;i<r->size();i++)
			(*r)[i]=i;

		rg.shuffle(*r); // shuffle elements of r

		for(unsigned int i=0;i<newRep->size();i++)
			(*newRep)[i] = (*r)[i];

		delete r;

		return * new TestSolution<RepTSP>(*newRep);
	}

};

#endif /*TSP_INITIALSOLUTION_Random_HPP_*/
