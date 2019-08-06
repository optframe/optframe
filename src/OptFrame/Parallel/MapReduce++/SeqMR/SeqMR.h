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

#ifndef SeqMR_HPP
#define SeqMR_HPP

#include "../mri/mri.h"

template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_Reducer;
class SeqMR_StrMapReduce;
class SeqMR_StrMapper;
class SeqMR_StrReducer;

#include "mapreduce.hpp"
#include "mapper.hpp"
#include "reducer.hpp"

#endif /* SeqMR_HPP */
