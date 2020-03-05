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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class EmptyLocalSearch : public LocalSearch<S, XEv>
{
public:

	EmptyLocalSearch()
	{
	}

	virtual ~EmptyLocalSearch()
	{
	}

	virtual void searchFrom(pair<S, XEv>&, const StopCriteria<XEv>& stopCriteria) override
   {
      // placeholder for empty local search
   };

	string log() const
	{
		return "Heuristic Empty: no log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<S, XEv>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class EmptyLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~EmptyLocalSearchBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		return new EmptyLocalSearch<S, XEv>;
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == EmptyLocalSearch<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":Empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_EMPTY_HPP_*/
