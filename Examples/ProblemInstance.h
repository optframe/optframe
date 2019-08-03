#ifndef KP2_PROBLEM_INSTANCE_H_
#define KP2_PROBLEM_INSTANCE_H_

#include <vector>
#include <iostream>

#ifdef DEBUG
#define PI_DEBUG 1
#endif

namespace KP2
{

class ProblemInstance{
public:
	int n, max_v;
	std::vector<int> value, weight;

	ProblemInstance();
};

}



#endif