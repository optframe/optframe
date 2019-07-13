#include "ConstructiveRandom.h"

using namespace PN;

ConstructiveRandom::ConstructiveRandom(ProblemInstance& _pPN): 
        pPN(_pPN)
{
}
	
ConstructiveRandom::~ConstructiveRandom()
{
}

Solution<RepPN, MY_ADS>& ConstructiveRandom::generateSolution()
{
    	RepPN sol = vector<bool>(pPN.nums.size());

	for (int i = 0; i < pPN.nums.size(); i++)
	{
		double u = (double) (rand()%100)/100.0;

		if (u <= 0.5)
			sol[i] = 0;
		else
			sol[i] = 1;
	}


    	return * new TestSolution<RepPN, MY_ADS>(sol);			
}
