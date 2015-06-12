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

#ifndef OPTFRAME_EMPTY_HPP_
#define OPTFRAME_EMPTY_HPP_

#include "../LocalSearch.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class EmptyLocalSearch : public LocalSearch<R, ADS>
{
public:

	EmptyLocalSearch()
	{
	}

	virtual ~EmptyLocalSearch()
	{
	}

	virtual void exec(Solution<R, ADS>&, double timelimit, double target_f){};
	virtual void exec(Solution<R, ADS>&, Evaluation&, double timelimit, double target_f){};

	string log()
	{
		return "Heuristic Empty: no log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EmptyLocalSearchBuilder : public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~EmptyLocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return new EmptyLocalSearch<R, ADS>;
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == EmptyLocalSearch<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_EMPTY_HPP_*/
