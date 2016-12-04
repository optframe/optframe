#include "Evaluator.h"

using namespace $project;

MyEvaluator::MyEvaluator(ProblemInstance& _p$project): // If necessary, add more parameters
p$project(_p$project)
{
	// Put the rest of your code here
}
	
MyEvaluator::~MyEvaluator()
{
}

Evaluation$project& MyEvaluator::evaluate(const Rep$project& rep)
{
    // 'rep' is the representation of the solution

    double fo = 0; // Evaluation Function Value

    return * new Evaluation$project(fo);
}

bool MyEvaluator::betterThan(double f1, double f2)
{
    // $minmax
    return $epsilon
}

bool MyEvaluator::isMinimization() const
{
	// $minmax
	return $isMin
}
