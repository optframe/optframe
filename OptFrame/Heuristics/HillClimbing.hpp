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

#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HillClimbing: public LocalSearch<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& evaluator;
	LocalSearch<R, ADS, M>& ls;

public:

	HillClimbing(Evaluator<R, ADS, M>& _ev, LocalSearch<R, ADS, M>& _ls) :
		evaluator(_ev), ls(_ls)
	{
	}

	virtual ~HillClimbing()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit,
			double target_f)
	{
		long tini = time(NULL);

		Solution<R, ADS>* s0 = &s.clone();
		Evaluation<M>* e0 = &e.clone();

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
		return (s == idComponent()) || (LocalSearch<R, ADS, M>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, M>::idComponent() << "HC";
		return ss.str();

	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HillClimbingBuilder : public LocalSearchBuilder<R, ADS, M>
{
public:
	virtual ~HillClimbingBuilder()
	{
	}

	virtual LocalSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "")
	{
		Evaluator<R, ADS, M>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, M>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, M>* h = method.first;

		scanner = Scanner(method.second);

		return new HillClimbing<R, ADS, M>(*eval, *h);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, M>::idComponent(), "evaluation function"));
		params.push_back(make_pair(LocalSearch<R, ADS, M>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == HillClimbing<R, ADS, M>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, M>::idComponent() << "HC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
