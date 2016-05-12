#include "Evaluator.h"

using namespace $project;

$projectEvaluator::$projectEvaluator(ProblemInstance& _p$project): // If necessary, add more parameters
p$project(_p$project)
{
	// Put the rest of your code here
}
	
$projectEvaluator::~$projectEvaluator()
{
}

Evaluation$project& $projectEvaluator::evaluate(const Rep$project& rep)
{
    // 'rep' is the representation of the solution

    double fo = 0; // Evaluation Function Value

    return * new Evaluation$project(fo);
}

bool $projectEvaluator::betterThan(double f1, double f2)
{
    // $minmax
    return $epsilon
}

bool $projectEvaluator::isMinimization() const
{
	// $minmax
	return $isMin
}
