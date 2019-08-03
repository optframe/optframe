#ifndef KP2_SOLUTION_H_
#define KP2_SOLUTION_H_

#include <vector>
#include <algorithm>
#include <iostream>

namespace KP2
{

class Solution{
public:
	std::vector<bool> item;

	Solution() = default;
	Solution(std::vector<bool>&& item);

};

}

std::ostream& operator<<(std::ostream& os, const KP2::Solution& sol); 

#endif