/*
 * NSEnumShiftk.hpp
 *
 *  Created on: 10/03/2010
 *      Author: sabir
 */

#ifndef NSENUMSHIFTK_HPP_
#define NSENUMSHIFTK_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSEnum.hpp"
#include "../../Util/NSEnumVVShiftk.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

using namespace std;

typedef MoveVVShiftk<int, MemVRPTW> MoveShiftk;
typedef NSEnumVVShiftk<int,MemVRPTW> NSEnumShiftk;

#endif /* NSENUMSHIFTK_HPP_ */
