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

#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class HillClimbing: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& evaluator;
	LocalSearch<R, ADS, DS>& ls;

public:

	HillClimbing(Evaluator<R, ADS, DS>& _ev, LocalSearch<R, ADS, DS>& _ls) :
		evaluator(_ev), ls(_ls)
	{
	}

	virtual ~HillClimbing()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = evaluator.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit,
			double target_f)
	{
		long tini = time(NULL);

		Solution<R, ADS>* s0 = &s.clone();
		Evaluation<DS>* e0 = &e.clone();

		ls.exec(s, e, timelimit, target_f);

		long tnow = time(NULL);

		while ((evaluator.betterThan(s, *s0)) && ((tnow - tini) < timelimit))
		{
			delete s0;
			s0 = &s.clone();
			delete e0;
			e0 = &e.clone();

			ls.exec(s, e, timelimit, target_f);

			tnow = time(NULL);
		}

		s = *s0;
		e = *e0;

		delete s0;
		delete e0;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "HC";
		return ss.str();

	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class HillClimbingBuilder : public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~HillClimbingBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, DS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, DS>* h = method.first;

		scanner = Scanner(method.second);

		return new HillClimbing<R, ADS, DS>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<R, ADS, DS>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == HillClimbing<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "HC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
