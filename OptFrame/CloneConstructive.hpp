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

#ifndef OPTFRAME_CLONE_CONSTRUCTIVE_HPP_
#define OPTFRAME_CLONE_CONSTRUCTIVE_HPP_

#include "Constructive.hpp"
#include "Solution.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CloneConstructive : public Constructive<R, ADS>
{
	Solution<R, ADS>& base;
public:

	CloneConstructive(Solution<R, ADS>& _base)
	: base(_base.clone())
	{
	}

	virtual ~CloneConstructive()
	{
		delete &base;
	}

	virtual Solution<R, ADS> generateSolution()
	{
		Solution<R,ADS>& s = base.clone();
		Solution<R,ADS> sc = s;
		delete &s;
		return sc;
	}

    virtual bool compatible(string s)
    {
    	return ( s == idComponent() ) || (Constructive<R, ADS>::compatible(s));
    }

	static string idComponent()
	{
		stringstream ss;
		ss << Constructive<R, ADS>::idComponent() << ":CloneConstructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CloneConstructiveBuilder : public ComponentBuilder<R, ADS>
{
public:
	virtual ~CloneConstructiveBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Solution<R, ADS>* s;
		hf.assign(s, scanner.nextInt(), scanner.next()); // reads backwards!

		return new CloneConstructive<R, ADS>(*s);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Solution<R, ADS>::idComponent(), "solution"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == CloneConstructive<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "CloneConstructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_CLONE_CONSTRUCTIVE_HPP_*/
