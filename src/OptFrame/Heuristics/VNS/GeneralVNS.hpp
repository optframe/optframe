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

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class GeneralVNS: public VariableNeighborhoodSearch<XES, XEv>
{
public:

	typedef VariableNeighborhoodSearch<XES, XEv> super;

	//GeneralVNS(Evaluator<S>& evaluator, Constructive<S>& constructive, vector<NS<XES, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
   GeneralVNS(GeneralEvaluator<XES>& evaluator, InitialSearch<XES>& constructive, vector<NS<XES, XEv>*> vshake, vector<NSSeq<XES, XEv>*> vsearch) :
		VariableNeighborhoodSearch<XES, XEv> (evaluator, constructive, vshake, vsearch)
	{
	}

	virtual ~GeneralVNS()
	{
	}

	virtual LocalSearch<XES, XEv>& buildSearch(unsigned k_search)
	{
		vector<LocalSearch<XES, XEv>* > vls;
		for(unsigned i=0; i<super::vsearch.size(); i++)
			vls.push_back(new BestImprovement<XES, XEv>(super::evaluator, *super::vsearch.at(i)));

		return * new VariableNeighborhoodDescent<XES, XEv>(super::evaluator, vls);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << VariableNeighborhoodSearch<XES, XEv>::idComponent() << "GVNS";
		return ss.str();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class GeneralVNSBuilder : public ILS, public SingleObjSearchBuilder<S, XEv, XES>
{
public:
	virtual ~GeneralVNSBuilder()
	{
	}

	virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		GeneralEvaluator<XES, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		//Constructive<S>* constructive;
      InitialSearch<XES>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<XES, XEv>*> shakelist;
		hf.assignList(shakelist, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NSSeq<XES, XEv>*> searchlist;
		hf.assignList(searchlist, scanner.nextInt(), scanner.next()); // reads backwards!


		return new BasicVNS<XES, XEv>(*eval, *constructive, shakelist, searchlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
		//params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));

		stringstream ss;
		ss << NS<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));

		stringstream ss2;
		ss2 << NSSeq<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss2.str(), "list of NSSeq"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicVNS<XES, XEv>::idComponent();
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
