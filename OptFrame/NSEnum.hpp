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

#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

#include "NSEnumIterator.hpp"

using namespace std;

template<class R, class ADS, class M> // already defined: ADS = OPTFRAME_DEFAULT_ADS , M = OPTFRAME_DEFAULT_EMEMORY
class NSEnum: public NSSeq<R, ADS, M>
{

protected:
	RandGen& rg;

public:

	using NSSeq<R, ADS, M>::move; // prevents name hiding

	NSEnum(RandGen& _rg):rg(_rg)
	{}

	virtual ~NSEnum()
	{
	}

	virtual Move<R, ADS, M>& move(const R&)
	{
		return move( rg.rand(size()));
	}

	virtual NSIterator<R, ADS, M>& getIterator(const R&)
	{
		return *new NSEnumIterator<R, ADS, M> (*this);
	}

	virtual Move<R, ADS, M>& move(unsigned int m) = 0;

	virtual unsigned int size() const = 0;

	virtual void print() const = 0;
};

#endif /*OPTFRAME_NSENUM_HPP_*/
