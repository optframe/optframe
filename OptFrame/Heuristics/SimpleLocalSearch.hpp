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

#ifndef OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_
#define OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.h"
#include "../SingleObjSearch.hpp"
#include "../LocalSearch.hpp"
#include "../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SimpleLocalSearch: public SingleObjSearch<R, ADS>
{
protected:
	Evaluator<R, ADS>& evaluator;
	Constructive<R, ADS>& constructive;
	LocalSearch<R, ADS>& localSearch;


public:

	SimpleLocalSearch(Evaluator<R, ADS>& _evaluator, Constructive<R, ADS>& _constructive, LocalSearch<R, ADS>& _localSearch) :
		evaluator(_evaluator), constructive(_constructive), localSearch(_localSearch)
	{
	}

	virtual ~SimpleLocalSearch()
	{
	}

	pair<Solution<R, ADS>&, Evaluation&>* search(double timelimit = 100000000, double target_f = 0,  const Solution<R, ADS>* _s = nullptr,  const Evaluation* _e = nullptr)
	{
		//cout << "SimpleLocalSearch search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation& e    = evaluator.evaluate(s);

                pair<Solution<R, ADS>&, Evaluation&>& p = localSearch.search(s, e, timelimit, target_f);

                delete &s;
                delete &e;

		return &p;
	}

	virtual bool compatible(string s)
	{
		return ( s == idComponent() ) || ( SingleObjSearch<R, ADS>::compatible(s) );
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << "SimpleLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << "SimpleLocalSearch with:" << endl;
		cout << "constructive: ";
		constructive.print();
		cout << "local search: ";
		localSearch.print();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<R, ADS>
{
public:
	virtual ~SimpleLocalSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);

		return new SimpleLocalSearch<R, ADS>(*eval, *constructive, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == SimpleLocalSearch<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << "SimpleLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_*/
