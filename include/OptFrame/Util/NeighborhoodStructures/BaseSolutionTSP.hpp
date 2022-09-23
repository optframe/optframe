#ifndef OPTFRAME_BASE_SOLUTION_TSP_HPP
#define OPTFRAME_BASE_SOLUTION_TSP_HPP

#include "../../Solution.hpp"

#include <vector>

namespace optframe {

// route can be of integers, shorts, or even double (permutation-style)
template<class T>
using BaseRouteTSP = std::vector<T>;

template<class T>
using BaseSoutionTSP = Solution<BaseRouteTSP<T>>;

}; // namespace optframe

#endif // OPTFRAME_BASE_SOLUTION_TSP_HPP
