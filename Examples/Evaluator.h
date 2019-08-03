#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <algorithm>
#include <vector>
#include "ProblemInstance.h"
#include "Solution.h"

#define BIG_M 10e5

#ifdef DEBUG
#define EV_DEBUG 1
#endif

namespace KP2
{

class Evaluator{
public:
	ProblemInstance& inst;

	Evaluator(ProblemInstance&);

	int evaluate(const Solution&);
	int evaluate(const std::vector<bool>&);
	#ifdef PRINT
	int check_weight(const std::vector<bool>&);
	#endif
};

}

#endif