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

#ifndef OPTFRAME_RDM_HPP_
#define OPTFRAME_RDM_HPP_

#include "../../LocalSearch.hpp"

#include "../../NS.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandomDescentMethod: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& evaluator;
	NS<R, ADS, DS>& ns;
	unsigned int iterMax;

public:

	RandomDescentMethod(Evaluator<R, ADS, DS>& _eval, NS<R, ADS, DS>& _ns, unsigned int _iterMax) :
		evaluator(_eval), ns(_ns), iterMax(_iterMax)
	{
	}

	virtual ~RandomDescentMethod()
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
		long tini = time(NULL);

		unsigned int iter = 0;

		long tnow = time(NULL);

		while ((iter < iterMax) && ((tnow - tini) < timelimit) && (evaluator.betterThan(target_f, e.evaluation())))
		{
			Move<R, ADS, DS>& move = ns.move(s);

			MoveCost* cost = NULL;

			if (move.canBeApplied(s))
			{
				cost = &evaluator.moveCost(e, move, s);
			}
			else
			{
				iter++;
				delete &move;
				tnow = time(NULL);
				continue;
			}

			iter++;

			if (cost && evaluator.isImprovement(*cost))
			{
				delete &move.apply(e, s);
				evaluator.evaluate(e, s);
				iter = 0;
			}

			if(cost)
				delete cost;

			delete &move;
			tnow = time(NULL);
		}
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandomDescentMethodBuilder : public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~RandomDescentMethodBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<R, ADS, DS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new RandomDescentMethod<R, ADS, DS>(*eval, *ns, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<R, ADS, DS>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandomDescentMethod<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_RDM_HPP_*/
