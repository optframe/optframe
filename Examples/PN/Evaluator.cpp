#include "Evaluator.h"

using namespace PN;

PNEvaluator::PNEvaluator(ProblemInstance& _pPN): // If necessary, add more parameters
pPN(_pPN)
{
	// Put the rest of your code here
}
	
PNEvaluator::~PNEvaluator()
{
}

EvaluationPN& PNEvaluator::evaluate(const RepPN& rep)
{
    evaluations++;
    double fo = 0, soma1=0, soma2=0;

    for(int i=0; i < rep.size(); i++)
    {
	if(rep[i] == 1)
		soma1 += pPN.nums[i];
	else
		soma2 += pPN.nums[i];
    }

    fo = abs(soma1 - soma2);

    return * new EvaluationPN(fo);
}

bool PNEvaluator::betterThan(double f1, double f2)
{
    // MINIMIZATION
    return (f1 < (f2 - EPSILON_PN));
}

