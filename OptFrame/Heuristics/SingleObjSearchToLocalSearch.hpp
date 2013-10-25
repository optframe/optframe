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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SingleObjSearchToLocalSearch: public LocalSearch<R, ADS, DS>
{
protected:
	Evaluator<R, ADS, DS>& evaluator;
	SingleObjSearch<R, ADS, DS>& sios;

public:

	SingleObjSearchToLocalSearch(Evaluator<R, ADS, DS>& _evaluator, SingleObjSearch<R, ADS, DS>& _sios) :
			evaluator(_evaluator), sios(_sios)
	{
	}

	virtual ~SingleObjSearchToLocalSearch()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = evaluator.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
		pair<Solution<R, ADS>&, Evaluation<DS>&>* r = sios.search(timelimit, target_f, &s, &e);

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
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, DS>::idComponent() << "SingleObjSearchToLocalSearch";
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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SingleObjSearchToLocalSearchBuilder: public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~SingleObjSearchToLocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<SingleObjSearch<R, ADS, DS>*, std::string> method;
		method = hf.createSingleObjSearch(rest);

		SingleObjSearch<R, ADS, DS>* h = method.first;

		scanner = Scanner(method.second);

		return new SingleObjSearchToLocalSearch<R, ADS, DS>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(SingleObjSearch<R, ADS, DS>::idComponent(), "single obj search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == SingleObjSearchToLocalSearch<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "SingleObjSearchToLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif
