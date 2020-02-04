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

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class HillClimbing: public LocalSearch<R, ADS, S>
{
private:
	Evaluator<R, ADS, S>& evaluator;
	LocalSearch<R, ADS, S>& ls;

public:

	HillClimbing(Evaluator<R, ADS, S>& _ev, LocalSearch<R, ADS, S>& _ls) :
		evaluator(_ev), ls(_ls)
	{
	}

	virtual ~HillClimbing()
	{
	}

	virtual void exec(S& s, SOSC& sosc) override
	{
		Evaluation e = evaluator.evaluateSolution(s);

		exec(s, e, sosc);
	}

	virtual void exec(S& s, Evaluation& e, SOSC& sosc) override
	{
      double timelimit = sosc.timelimit;

		long tini = time(nullptr);

		Evaluation* e0 = &e.clone();

		ls.exec(s, e, sosc);

		long tnow = time(nullptr);

		while ((evaluator.betterThan(e, *e0)) && ((tnow - tini) < timelimit))
		{
			//delete e0;
			//e0 = &e.clone();
            (*e0) = e;

			ls.exec(s, e, sosc);

			tnow = time(nullptr);
		}	

		delete e0;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, S>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, S>::idComponent() << ":HC";
		return ss.str();

	}
};


template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class HillClimbingBuilder : public LocalSearchBuilder<R, ADS, S>
{
public:
	virtual ~HillClimbingBuilder()
	{
	}

	virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS, S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, S>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, S>* h = method.first;

		scanner = Scanner(method.second);

		return new HillClimbing<R, ADS, S>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<R, ADS, S>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == HillClimbing<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, S>::idComponent() << ":HC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
