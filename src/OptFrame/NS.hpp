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

template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
class NS: public Component
{
public:

	virtual ~NS()
	{
	}

public:


	virtual Move<S, XEv>* randomMove(const S&) = 0;

	virtual Move<S, XEv>* validRandomMove(const S& s)
	{
		Move<S, XEv>* moveValid = this->randomMove(s);
		if(moveValid->canBeApplied(s))
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
