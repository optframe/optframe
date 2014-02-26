// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef TSP_NSENUMShift_HPP_
#define TSP_NSENUMShift_HPP_

// Framework includes
#include "../../OptFrame/Move.hpp"
#include "../../OptFrame/NSEnum.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSEnumVectorShift.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Memory.h"
#include "Solution.h"

using namespace std;

typedef MoveVectorShift<int, MemTSP> MoveShiftTSP;
typedef NSEnumVectorShift<int, MemTSP> NSEnumShiftTSP;

#endif /*TSP_NSENUMShift_HPP_*/
