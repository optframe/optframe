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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.h"
#include "../SingleObjSearch.hpp"
#include "../LocalSearch.hpp"
#include "../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SingleObjSearchToLocalSearch: public LocalSearch<R, ADS>
{
protected:
	Evaluator<R, ADS>& evaluator;
	SingleObjSearch<R, ADS>& sios;

public:

	SingleObjSearchToLocalSearch(Evaluator<R, ADS>& _evaluator, SingleObjSearch<R, ADS>& _sios) :
			evaluator(_evaluator), sios(_sios)
	{
	}

	virtual ~SingleObjSearchToLocalSearch()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation& e = evaluator.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		pair<Solution<R, ADS>&, Evaluation&>* r = sios.search(timelimit, target_f, &s, &e);

		if(r)
		{
			s=r->first;
			e=r->second;
			delete& r->first;
			delete& r->second;
			delete r;
		}
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << "SingleObjSearchToLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << "SingleObjSearchToLocalSearch with sios: ";
		sios.print();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SingleObjSearchToLocalSearchBuilder: public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~SingleObjSearchToLocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<SingleObjSearch<R, ADS>*, std::string> method;
		method = hf.createSingleObjSearch(rest);

		SingleObjSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);

		return new SingleObjSearchToLocalSearch<R, ADS>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(SingleObjSearch<R, ADS>::idComponent(), "single obj search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == SingleObjSearchToLocalSearch<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << "SingleObjSearchToLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif
