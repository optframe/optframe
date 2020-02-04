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

#ifndef OPTFRAME_BASIC_VNS_HPP_
#define OPTFRAME_BASIC_VNS_HPP_

#include <math.h>
#include <vector>

#include "../LocalSearches/BestImprovement.hpp"
#include "../../HeuristicFactory.hpp"
#include "VariableNeighborhoodSearch.hpp"
#include "VNS.h"

namespace optframe
{

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicVNS: public VariableNeighborhoodSearch<R, ADS, S>
{
public:

	typedef VariableNeighborhoodSearch<R, ADS> super;

	BasicVNS(Evaluator<R, ADS>& evaluator, Constructive<S>& constructive, vector<NS<R, ADS>*> vshake, vector<NSSeq<R, ADS>*> vsearch) :
		VariableNeighborhoodSearch<R, ADS> (evaluator, constructive, vshake, vsearch)
	{
	}

	virtual ~BasicVNS()
	{
	}

	virtual LocalSearch<R, ADS>& buildSearch(unsigned k_search)
	{
		return * new BestImprovement<R, ADS>(super::evaluator, *super::vsearch.at(k_search));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << VariableNeighborhoodSearch<R, ADS >::idComponent() << "BVNS";
		return ss.str();
	}
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicVNSBuilder : public ILS, public SingleObjSearchBuilder<R, ADS, S>
{
public:
	virtual ~BasicVNSBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<R, ADS>*> shakelist;
		hf.assignList(shakelist, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NSSeq<R, ADS>*> searchlist;
		hf.assignList(searchlist, scanner.nextInt(), scanner.next()); // reads backwards!


		return new BasicVNS<R, ADS>(*eval, *constructive, shakelist, searchlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));

		stringstream ss;
		ss << NS<R, ADS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));

		stringstream ss2;
		ss2 << NSSeq<R, ADS>::idComponent() << "[]";
		params.push_back(make_pair(ss2.str(), "list of NSSeq"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicVNS<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << VNS::family() << "BasicVNS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BasicVNS_HPP_*/
