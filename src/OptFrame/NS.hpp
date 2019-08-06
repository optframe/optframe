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

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "Component.hpp"
#include "Action.hpp"

namespace optframe
{

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class NS: public Component
{
public:

	virtual ~NS()
	{
	}

public:
	Move<R, ADS, S>* randomMoveSolution(const S& s)
	{
		return randomMove(s.getR(), s.getADSptr());
	}

	Move<R, ADS, S>* validRandomMoveSolution(const S& s)
	{
		return validRandomMove(s.getR(), s.getADSptr());
	}

////protected:
	virtual Move<R, ADS, S>* randomMove(const R&, const ADS*) = 0;

	virtual Move<R, ADS, S>* validRandomMove(const R& r, const ADS* ads)
	{
		Move<R, ADS, S>* moveValid = this->randomMove(r, ads);
		if(moveValid->canBeApplied(r, ads))
			return moveValid;
		else
		{
			delete moveValid;
			return nullptr;
		}
	}

public:
	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":NS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}
};

}

#endif /*OPTFRAME_NS_HPP_*/
