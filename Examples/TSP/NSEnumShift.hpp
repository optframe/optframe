#ifndef TSP_NSENUMShift_HPP_
#define TSP_NSENUMShift_HPP_

// Framework includes
#include "../../OptFrame/Move.hpp"
#include "../../OptFrame/NSEnum.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSEnumVectorShift.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

using namespace std;

typedef MoveVectorShift<int, MemTSP> MoveShiftTSP;
typedef NSEnumVectorShift<int, MemTSP> NSEnumShiftTSP;

#endif /*TSP_NSENUMShift_HPP_*/
