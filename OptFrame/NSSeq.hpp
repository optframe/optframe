// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NS.hpp"
#include "NSIterator.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeq: public NS<R, M>
{
public:

	using NS<R, M>::move; // prevents name hiding

	virtual ~NSSeq()
	{
	}

	virtual Move<R, M>& move(const R&) = 0;

	virtual NSIterator<R, M>& getIterator(const R&) = 0;
	virtual NSIterator<R, M>& getIterator(const M&, const R& r)
	{
		return getIterator(r);
	}

	virtual void print() = 0;
};

#endif /*OPTFRAME_NSSEQ_HPP_*/
