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
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"

namespace optframe
{

template<XSolution S, XEvaluation XEv=Evaluation<>>
class CompareLocalSearch: public LocalSearch<S, XEv>
{
private:
	Evaluator<S, XEv>& eval;
	LocalSearch<S, XEv>& ls1;
	LocalSearch<S, XEv>& ls2;

public:

	CompareLocalSearch(Evaluator<S, XEv>& _eval, LocalSearch<S, XEv>& _ls1,  LocalSearch<S, XEv>& _ls2) :
		eval(_eval), ls1(_ls1), ls2(_ls2)
	{
	}

	virtual ~CompareLocalSearch()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, SOSC<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(pair<S, XEv>& se, SOSC<XEv>& sosc) override
	{
      //S& s = se.first;
      XEv& e = se.second;
		
      //S& s2 = s.clone();
		//Evaluation<>& e2   = e.clone();
      pair<S, XEv> p2 = se; // clone!
      XEv& e2 = p2.second;

		ls1.searchFrom(se, sosc);
		ls2.searchFrom(p2, sosc);

		if(!eval.equals(e, e2))
		{
			cout << "CompareLocalSearch error: difference between " << e.evaluation() << " and " << e2.evaluation() << endl;
			cout << "LocalSearch 1: ";
			ls1.print();
			cout << "LocalSearch 2: ";
			ls2.print();
			exit(1);
		}

		//delete &s2;
		//delete &e2;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<S, XEv>::idComponent() << "CompareLocalSearch";
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


template<XSolution S, XEvaluation XEv=Evaluation<>>
class CompareLocalSearchBuilder : public LocalSearchBuilder<S, XEv>
{
public:
	virtual ~CompareLocalSearchBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<S, XEv>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<S, XEv>* h = method.first;

		scanner = Scanner(method.second);

		string rest2 = scanner.rest();

		pair<LocalSearch<S, XEv>*, std::string> method2;
		method2 = hf.createLocalSearch(rest2);

		LocalSearch<S, XEv>* h2 = method2.first;

		scanner = Scanner(method2.second);

		return new CompareLocalSearch<S, XEv>(*eval, *h, *h2);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<S, XEv>::idComponent(), "local search 1"));
		params.push_back(make_pair(LocalSearch<S, XEv>::idComponent(), "local search 2"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovement<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << "CompareLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_*/
