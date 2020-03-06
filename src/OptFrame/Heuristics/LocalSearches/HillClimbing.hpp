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

#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class HillClimbing: public LocalSearch<XES, XEv>
{
private:
	Evaluator<XES, XEv>& evaluator;
	LocalSearch<XES, XEv>& ls;

public:

	HillClimbing(Evaluator<XES, XEv>& _ev, LocalSearch<XES, XEv>& _ls) :
		evaluator(_ev), ls(_ls)
	{
	}

	virtual ~HillClimbing()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(XES& se, const StopCriteria<XES>& sosc) override
	{
      //S& s = se.first;
      XEv& e = se.second;
      //
      double timelimit = sosc.timelimit;

		long tini = time(nullptr);

		//Evaluation<>* e0 = &e.clone();
      XEv e0(e); // avoid that using return status on 'exec'

		ls.searchFrom(se, sosc);

		long tnow = time(nullptr);

      // while improvement is found
		while ((evaluator.betterThan(e, e0)) && ((tnow - tini) < timelimit))
		{
			//delete e0;
			//e0 = &e.clone();

         //   (*e0) = e;
         e0 = e;

			ls.searchFrom(se, sosc);

			tnow = time(nullptr);
		}	

		//delete e0;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << ":HC";
		return ss.str();

	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class HillClimbingBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~HillClimbingBuilder()
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

		return new HillClimbing<XES, XEv>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == HillClimbing<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":HC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
