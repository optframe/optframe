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

#ifndef OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_
#define OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_

#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CompareLocalSearch: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& eval;
	LocalSearch<R, ADS>& ls1;
	LocalSearch<R, ADS>& ls2;

public:

	CompareLocalSearch(Evaluator<R, ADS>& _eval, LocalSearch<R, ADS>& _ls1,  LocalSearch<R, ADS>& _ls2) :
		eval(_eval), ls1(_ls1), ls2(_ls2)
	{
	}

	virtual ~CompareLocalSearch()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation& e = eval.evaluate(s);
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		Solution<R, ADS>& s2 = s.clone();
		Evaluation& e2   = e.clone();

		ls1.exec(s, e, timelimit, target_f);
		ls2.exec(s2, e2, timelimit, target_f);

		if(!eval.equals(e, e2))
		{
			cout << "CompareLocalSearch error: difference between " << e.evaluation() << " and " << e2.evaluation() << endl;
			cout << "LocalSearch 1: ";
			ls1.print();
			cout << "LocalSearch 2: ";
			ls2.print();
			exit(1);
		}

		delete &s2;
		delete &e2;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << "CompareLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "CLS: (" << ls1.toString() << "," << ls2.toString() << ")";
		return ss.str();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CompareLocalSearchBuilder : public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~CompareLocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);

		string rest2 = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method2;
		method2 = hf.createLocalSearch(rest2);

		LocalSearch<R, ADS>* h2 = method2.first;

		scanner = Scanner(method2.second);

		return new CompareLocalSearch<R, ADS>(*eval, *h, *h2);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search 1"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search 2"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovement<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << "CompareLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_*/
