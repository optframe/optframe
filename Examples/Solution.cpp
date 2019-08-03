#include "Solution.h"

using namespace KP2;

Solution::Solution(std::vector<bool>&& _item){
	std::swap(item, _item);
}

std::ostream& operator<<(std::ostream& os, const Solution& sol){
	std::for_each(sol.item.begin(), sol.item.end(), [&os](const bool x){ os << x;} );
	return os;
}