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

template<XESolution XES, XEvaluation XEv=Evaluation<>>
class CompareLocalSearch: public LocalSearch<XES, XEv>
{
private:
	Evaluator<XES, XEv>& eval;
	LocalSearch<XES, XEv>& ls1;
	LocalSearch<XES, XEv>& ls2;

public:

	CompareLocalSearch(Evaluator<XES, XEv>& _eval, LocalSearch<XES, XEv>& _ls1,  LocalSearch<XES, XEv>& _ls2) :
		eval(_eval), ls1(_ls1), ls2(_ls2)
	{
	}

	virtual ~CompareLocalSearch()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override
	{
      //S& s = se.first;
      XEv& e = se.second;
		
      //S& s2 = s.clone();
		//Evaluation<>& e2   = e.clone();
      XES p2 = se; // clone!
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
		return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << "CompareLocalSearch";
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


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class CompareLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~CompareLocalSearchBuilder()
	{
	}

	virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		Evaluator<XES, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<XES, XEv>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<XES, XEv>* h = method.first;

		scanner = Scanner(method.second);

		string rest2 = scanner.rest();

		pair<LocalSearch<XES, XEv>*, std::string> method2;
		method2 = hf.createLocalSearch(rest2);

		LocalSearch<XES, XEv>* h2 = method2.first;

		scanner = Scanner(method2.second);

		return new CompareLocalSearch<XES, XEv>(*eval, *h, *h2);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search 1"));
		params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search 2"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovement<XES, XEv>::idComponent();
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
