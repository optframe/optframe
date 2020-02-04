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

#include <vector>

#include "../Evaluation.hpp"
#include "../LocalSearch.hpp"

namespace optframe
{

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class EmptyLocalSearch : public LocalSearch<R, ADS, S>
{
public:

	EmptyLocalSearch()
	{
	}

	virtual ~EmptyLocalSearch()
	{
	}

	virtual void exec(S&, SOSC& stopCriteria){};
	virtual void exec(S&, Evaluation<>&, SOSC& stopCriteria){};

	string log() const
	{
		return "Heuristic Empty: no log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, S>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, S>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class EmptyLocalSearchBuilder : public LocalSearchBuilder<R, ADS, S>
{
public:
	virtual ~EmptyLocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		return new EmptyLocalSearch<R, ADS, S>;
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == EmptyLocalSearch<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, S>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_EMPTY_HPP_*/
