/*
 * NSEnumSwapk.hpp
 *
 *  Created on: 16/03/2010
 *      Author: sabir
 */

#ifndef NSENUMSWAPK_HPP_
#define NSENUMSWAPK_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSEnum.hpp"
#include "../../Util/NSEnumVVSwapk.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

using namespace std;

typedef MoveVVSwapk<int, MemVRPTW> MoveSwapk;
typedef NSEnumVVSwapk<int,MemVRPTW> NSEnumSwapk;

#endif /* NSENUMSWAPK_HPP_ */
