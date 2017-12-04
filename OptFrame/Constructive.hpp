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

#ifndef OPTFRAME_CONSTRUCTIVE_HPP_
#define OPTFRAME_CONSTRUCTIVE_HPP_

#include "Solution.hpp"

namespace optframe
{


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Constructive : public Component
{
public:
	virtual ~Constructive()
	{
	}

	// timelimit in seconds, accepting fractions (millisecs, ...)
	virtual Solution<R, ADS>* generateSolution(double timelimit) = 0;

    virtual bool compatible(string s)
    {
    	return ( s == idComponent() ) || (Component::compatible(s));
    }

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Constructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


}

#endif /*OPTFRAME_CONSTRUCTIVE_HPP_*/
