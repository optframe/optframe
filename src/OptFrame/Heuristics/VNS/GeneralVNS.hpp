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

#ifndef OPTFRAME_GENERAL_VNS_HPP_
#define OPTFRAME_GENERAL_VNS_HPP_

#include <math.h>
#include <vector>

#include "VariableNeighborhoodSearch.hpp"
#include "VNS.h"

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class GeneralVNS: public VariableNeighborhoodSearch<S, XEv>
{
public:

	typedef VariableNeighborhoodSearch<S, XEv> super;

	GeneralVNS(Evaluator<S>& evaluator, Constructive<S>& constructive, vector<NS<S, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
		VariableNeighborhoodSearch<S, XEv> (evaluator, constructive, vshake, vsearch)
	{
	}

	virtual ~GeneralVNS()
	{
	}

	virtual LocalSearch<S, XEv>& buildSearch(unsigned k_search)
	{
		vector<LocalSearch<S, XEv>* > vls;
		for(unsigned i=0; i<super::vsearch.size(); i++)
			vls.push_back(new BestImprovement<S, XEv>(super::evaluator, *super::vsearch.at(i)));

		return * new VariableNeighborhoodDescent<S, XEv>(super::evaluator, vls);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << VariableNeighborhoodSearch<S, XEv>::idComponent() << "GVNS";
		return ss.str();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>>
class GeneralVNSBuilder : public ILS, public SingleObjSearchBuilder<S, XEv>
{
public:
	virtual ~GeneralVNSBuilder()
	{
	}

	virtual SingleObjSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv>& hf, string family = "")
	{
		Evaluator<S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<S, XEv>*> shakelist;
		hf.assignList(shakelist, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NSSeq<S>*> searchlist;
		hf.assignList(searchlist, scanner.nextInt(), scanner.next()); // reads backwards!


		return new BasicVNS<S, XEv>(*eval, *constructive, shakelist, searchlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));

		stringstream ss;
		ss << NS<S, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));

		stringstream ss2;
		ss2 << NSSeq<S>::idComponent() << "[]";
		params.push_back(make_pair(ss2.str(), "list of NSSeq"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicVNS<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family() << "GVNS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_GENERAL_VNS_HPP_*/
