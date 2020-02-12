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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class HillClimbing: public LocalSearch<S, XEv>
{
private:
	Evaluator<S, XEv>& evaluator;
	LocalSearch<S, XEv>& ls;

public:

	HillClimbing(Evaluator<S, XEv>& _ev, LocalSearch<S, XEv>& _ls) :
		evaluator(_ev), ls(_ls)
	{
	}

	virtual ~HillClimbing()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, SOSC& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void exec(pair<S, XEv>& se, SOSC& sosc) override
	{
      //S& s = se.first;
      XEv& e = se.second;
      //
      double timelimit = sosc.timelimit;

		long tini = time(nullptr);

		//Evaluation<>* e0 = &e.clone();
      XEv e0(e); // avoid that using return status on 'exec'

		ls.exec(se, sosc);

		long tnow = time(nullptr);

      // while improvement is found
		while ((evaluator.betterThan(e, e0)) && ((tnow - tini) < timelimit))
		{
			//delete e0;
			//e0 = &e.clone();

         //   (*e0) = e;
         e0 = e;

			ls.exec(se, sosc);

			tnow = time(nullptr);
		}	

		//delete e0;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<S, XEv>::idComponent() << ":HC";
		return ss.str();

	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>>
class HillClimbingBuilder : public LocalSearchBuilder<S, XEv>
{
public:
	virtual ~HillClimbingBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<S, XEv>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<S, XEv>* h = method.first;

		scanner = Scanner(method.second);

		return new HillClimbing<S, XEv>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<S, XEv>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == HillClimbing<S, XEv>::idComponent();
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
