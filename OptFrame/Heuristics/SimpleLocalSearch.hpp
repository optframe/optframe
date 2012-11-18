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

#ifndef OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_
#define OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.h"
#include "../SingleObjSearch.hpp"
#include "../LocalSearch.hpp"
#include "../Evaluator.hpp"


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class SimpleLocalSearch: public SingleObjSearch<R, ADS, M>
{
protected:
	Evaluator<R, ADS, M>& evaluator;
	Constructive<R, ADS>& constructive;
	LocalSearch<R, ADS, M>& localSearch;


public:

	SimpleLocalSearch(Evaluator<R, ADS, M>& _evaluator, Constructive<R, ADS>& _constructive, LocalSearch<R, ADS, M>& _localSearch) :
		evaluator(_evaluator), constructive(_constructive), localSearch(_localSearch)
	{
	}

	virtual ~SimpleLocalSearch()
	{
	}

	pair<Solution<R, ADS>&, Evaluation<M>&>* search(double timelimit = 100000000, double target_f = 0)
	{
		cout << "SimpleLocalSearch search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation<M>& e    = evaluator.evaluate(s);

                pair<Solution<R, ADS>&, Evaluation<M>&>& p = localSearch.search(s, e, timelimit, target_f);

                delete &s;
                delete &e;

		return &p;
	}

	virtual bool compatible(string s)
	{
		return ( s == idComponent() ) || ( SingleObjSearch<R, ADS, M>::compatible(s) );
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, M>::idComponent() << "SimpleLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<R, ADS, M>
{
public:
	virtual ~SimpleLocalSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "")
	{
		Evaluator<R, ADS, M>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, M>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, M>* h = method.first;

		scanner = Scanner(method.second);

		return new SimpleLocalSearch<R, ADS, M>(*eval, *constructive, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, M>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS, M>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == SimpleLocalSearch<R, ADS, M>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, M>::idComponent() << "SimpleLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


#endif /*OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_*/
