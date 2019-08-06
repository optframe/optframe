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

#ifndef MaPI_HPP
#define MaPI_HPP

#include "../mri/mri.h"

template<class KeyA, class A, class KeyB, class B, class C> class MaPI_MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Reducer;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Serializer;
class MaPI_StrMapReduce;
class MaPI_StrMapper;
class MaPI_StrReducer;

#include <mpi.h>
#include <cstdlib>
#include <sstream>
#include "../util/Scanner++/Scanner.h"
#include "serializer.hpp"
#include "mapreduce.hpp"
#include "mapper.hpp"
#include "reducer.hpp"
#include "fsmapreduce.hpp"

#endif /* MaPI_HPP */
