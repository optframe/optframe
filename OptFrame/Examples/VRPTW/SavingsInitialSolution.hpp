#ifndef VRPTW_INITIALSOLUTION_Savings_HPP_
#define VRPTW_INITIALSOLUTION_Savings_HPP_

#include "../../InitialSolution.h"
#include "../../Solution/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

class SavingsInitialSolutionVRPTW: public InitialSolution<RepVRPTW>
{
private:
   VRPTWProblemInstance* pI;

   // Your private vars

public:
	
	SavingsInitialSolutionVRPTW(VRPTWProblemInstance* pI) // If necessary, add more parameters
        {
           this->pI = pI;

           // Put the rest of your code here
	};

	SolutionVRPTW* generateSolution()
	{
	   RepVRPTW* newRep = NULL;

	   return new TestSolution<RepVRPTW>(newRep);			
	}
	
};

#endif /*VRPTW_INITIALSOLUTION_Savings_HPP_*/
