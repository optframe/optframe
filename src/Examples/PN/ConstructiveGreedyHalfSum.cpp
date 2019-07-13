#include "ConstructiveGreedyHalfSum.h"

using namespace PN;

ConstructiveGreedyHalfSum::ConstructiveGreedyHalfSum(ProblemInstance& _pPN): 
        pPN(_pPN)
{
}
	
ConstructiveGreedyHalfSum::~ConstructiveGreedyHalfSum()
{
}

Solution<RepPN, MY_ADS>& ConstructiveGreedyHalfSum::generateSolution()
{
    	RepPN sol = vector<bool>(pPN.nums.size());

	double soma=0, soma1=0, soma2=0;

	random_shuffle(pPN.nums.begin(), pPN.nums.end());

	for (int i = 0; i < pPN.nums.size(); ++i)
	{
		soma += pPN.nums[i];
	}

	soma /= 2;

	int i = 0;

	while((soma1 + pPN.nums[i]) <= soma)
	{
		sol[i] = 1;
		soma1 += pPN.nums[i];
		i++;
	}

	for (int j = i; j < pPN.nums.size(); ++j)
	{
		if((soma1 + pPN.nums[j]) <= soma)
		{
			sol[j] = 1;
			soma1 += pPN.nums[j];
		}
		else
		{
			sol[j] = 0;
			soma2 += pPN.nums[j];
		}
	}


    	return * new TestSolution<RepPN, MY_ADS>(sol);			
}
