#include "../../Examples/SCP/Evaluator.h"

using namespace SCP;

SCPEvaluator::SCPEvaluator(ProblemInstance& _pSCP): // If necessary, add more parameters
pSCP(_pSCP)
{
	// Put the rest of your code here
}
	
SCPEvaluator::~SCPEvaluator()
{
}

EvaluationSCP& SCPEvaluator::evaluate(const RepSCP& rep)
{
    // 'rep' is the representation of the solution

    double fo = 0; // Evaluation Function Value

    return * new EvaluationSCP(fo  , * new int);
}

bool SCPEvaluator::betterThan(double f1, double f2)
{
    // MAXIMIZATION
    return (f1 > (f2 + EPSILON_SCP));
}

bool SCPEvaluator::isMinimization() const
{
	// MAXIMIZATION
	return false;
}
