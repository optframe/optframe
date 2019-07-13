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

#ifndef OPTFRAME_RDM_HPP_
#define OPTFRAME_RDM_HPP_

#include "../../LocalSearch.hpp"

#include "../../NS.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class RandomDescentMethod: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& evaluator;
	NS<R, ADS>& ns;
	unsigned int iterMax;

public:

	RandomDescentMethod(Evaluator<R, ADS>& _eval, NS<R, ADS>& _ns, unsigned int _iterMax) :
		evaluator(_eval), ns(_ns), iterMax(_iterMax)
	{
	}

	virtual ~RandomDescentMethod()
	{
	}

	virtual void exec(Solution<R, ADS>& s, SOSC& stopCriteria)
	{
		Evaluation e = evaluator.evaluateSolution(s);
		exec(s, e, stopCriteria);
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria)
	{
		Timer tNow;

		unsigned int iter = 0;

		while ((iter < iterMax) && (tNow.now() < stopCriteria.timelimit) && (evaluator.betterThan(stopCriteria.target_f, e.evaluation())))
		{
			// TODO: verify if it's not null!
			Move<R, ADS>& move = *ns.randomMoveSolution(s);

			MoveCost* cost = nullptr;

			if (move.canBeAppliedToSolution(s))
			{
				cost = evaluator.moveCost(e, move, s);
			}
			else
			{
				iter++;
				delete &move;
				continue;
			}

			iter++;

			if (cost && evaluator.isImprovement(*cost))
			{
				Component::safe_delete(move.applyUpdateSolution(e, s));
				evaluator.reevaluateSolution(e, s);
				iter = 0;
			}

			if(cost)
				delete cost;

			delete &move;
		}
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class RandomDescentMethodBuilder : public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~RandomDescentMethodBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<R, ADS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new RandomDescentMethod<R, ADS>(*eval, *ns, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<R, ADS>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandomDescentMethod<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_RDM_HPP_*/
