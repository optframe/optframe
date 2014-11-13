#include "ConstructiveGreedySum.h"

using namespace PN;

ConstructiveGreedySum::ConstructiveGreedySum(ProblemInstance& _pPN): 
        pPN(_pPN)
{
}
	
ConstructiveGreedySum::~ConstructiveGreedySum()
{
}

Solution<RepPN, MY_ADS>& ConstructiveGreedySum::generateSolution()
{
	RepPN sol = vector<bool>(pPN.nums.size());

	double soma1=0, soma2=0;

	sort(pPN.nums.rbegin(),pPN.nums.rend());

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
