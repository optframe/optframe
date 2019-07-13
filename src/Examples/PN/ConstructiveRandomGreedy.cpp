#include "ConstructiveRandomGreedy.h"

using namespace PN;

ConstructiveRandomGreedy::ConstructiveRandomGreedy(ProblemInstance& _pPN): 
        pPN(_pPN)
{
}
	
ConstructiveRandomGreedy::~ConstructiveRandomGreedy()
{
}

Solution<RepPN, MY_ADS>& ConstructiveRandomGreedy::generateSolution()
{
    	RepPN sol = vector<bool>(pPN.nums.size());

	double soma=0, soma1=0, soma2=0;

	random_shuffle(pPN.nums.rbegin(), pPN.nums.rend());

	for (int i = 0; i < pPN.nums.size(); i++)
	{
		if (soma1 > soma2)
		{
			sol[i] = 0;
			soma2 += pPN.nums[i];
		}
		else
		{
			sol[i] = 1;
			soma1 += pPN.nums[i];
		}
	}


    	return * new TestSolution<RepPN, MY_ADS>(sol);			
}
