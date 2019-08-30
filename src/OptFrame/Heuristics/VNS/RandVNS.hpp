// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_RAND_VNS_HPP_
#define OPTFRAME_RAND_VNS_HPP_

#include <math.h>
#include <vector>

#include "VariableNeighborhoodSearch.hpp"
#include "VNS.h"
#include "../LocalSearches/RVND.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandVNS: public VariableNeighborhoodSearch<R, ADS, DS>
{
public:

	typedef VariableNeighborhoodSearch<R, ADS, DS> super;

	RandVNS(Evaluator<R, ADS, DS>& evaluator, Constructive<R, ADS>& constructive, vector<NS<R, ADS, DS>*> vshake, vector<NSSeq<R, ADS, DS>*> vsearch, RandGen& _rg) :
		VariableNeighborhoodSearch<R, ADS, DS> (evaluator, constructive, vshake, vsearch), rg(_rg)
	{
	}

	virtual ~RandVNS()
	{
	}

	virtual LocalSearch<R, ADS, DS>& buildSearch(unsigned k_search)
	{
		vector<LocalSearch<R, ADS, DS>* > vls;
		for(unsigned i=0; i<super::vsearch.size(); i++)
			vls.push_back(new BestImprovement<R, ADS, DS>(super::evaluator, *super::vsearch.at(i)));

		return * new RVND<R, ADS, DS>(super::evaluator, vls, rg);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << VariableNeighborhoodSearch<R, ADS, DS >::idComponent() << "RandVNS";
		return ss.str();
	}
private:
	RandGen& rg;
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandVNSBuilder : public ILS, public SingleObjSearchBuilder<R, ADS, DS>
{
public:
	virtual ~RandVNSBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<R, ADS, DS>*> shakelist;
		hf.assignList(shakelist, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NSSeq<R, ADS, DS>*> searchlist;
		hf.assignList(searchlist, scanner.nextInt(), scanner.next()); // reads backwards!


		return new RandVNS<R, ADS, DS>(*eval, *constructive, shakelist, searchlist, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));

		stringstream ss;
		ss << NS<R, ADS, DS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));

		stringstream ss2;
		ss2 << NSSeq<R, ADS, DS>::idComponent() << "[]";
		params.push_back(make_pair(ss2.str(), "list of NSSeq"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandVNS<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, DS>::idComponent() << VNS::family() << "RandVNS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_GENERAL_VNS_HPP_*/
