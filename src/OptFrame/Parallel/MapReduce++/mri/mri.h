// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef MRI_H
#define MRI_H

#include <map>
#include <vector>
#include <string>

using namespace std;

template<class KeyA, class A, class KeyB, class B, class C> class MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class Reducer;
class StrMapReduce;
class StrMapper;
class StrReducer;

#include "mapreduce.h"
#include "mapper.h"
#include "reducer.h"
#include "printable.h"

#endif /* MRI_H */
