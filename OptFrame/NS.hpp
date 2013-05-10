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

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "OptFrameComponent.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NS: public OptFrameComponent
{
public:

	virtual ~NS()
	{
	}

	Move<R, ADS, DS>& move(const Solution<R, ADS>& s)
	{
		return move(s.getR(), s.getADS());
	}

	Move<R, ADS, DS>* validMove(const Solution<R, ADS>& s)
	{
		return validMove(s.getR(), s.getADS());
	}

	virtual Move<R, ADS, DS>& move(const R&, const ADS&) = 0;

	virtual Move<R, ADS, DS>* validMove(const R& r, const ADS& ads)
	{
		Move<R, ADS, DS>* moveValid = &(this->move(r, ads));
		if (moveValid->canBeApplied(r, ads))
			return moveValid;
		else
			delete moveValid;

		return NULL;
	}
	static string idComponent()
	{
		return "OptFrame:NS";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (OptFrameComponent::compatible(s));
	}
};

#endif /*OPTFRAME_NS_HPP_*/
