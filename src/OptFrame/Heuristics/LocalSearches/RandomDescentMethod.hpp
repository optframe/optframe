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
#include "../../Timer.hpp"

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class RandomDescentMethod: public LocalSearch<S, XEv>
{
private:
	Evaluator<S, XEv>& evaluator;
	NS<S, XEv>& ns;
	unsigned int iterMax;

public:

	RandomDescentMethod(Evaluator<S, XEv>& _eval, NS<S, XEv>& _ns, unsigned int _iterMax) :
		evaluator(_eval), ns(_ns), iterMax(_iterMax)
	{
	}

	virtual ~RandomDescentMethod()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(pair<S, XEv>& se, const StopCriteria<XEv>& stopCriteria) override
	{
      S& s = se.first;
      XEv& e = se.second;
		Timer tNow;

		unsigned int iter = 0;

		while ((iter < iterMax) && (tNow.now() < stopCriteria.timelimit) && (evaluator.betterThan(stopCriteria.target_f, e)))
		{
			// TODO: verify if it's not null!
			Move<S, XEv>& move = *ns.randomMove(s);

			MoveCost<>* cost = nullptr;

			if (move.canBeApplied(s))
			{
				cost = evaluator.moveCost(move, se);
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
				Component::safe_delete(move.applyUpdate(se));
				evaluator.reevaluate(se);
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
		ss << LocalSearch<S, XEv>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class RandomDescentMethodBuilder : public LocalSearchBuilder<S, XEv, X2ES>
{
public:
	virtual ~RandomDescentMethodBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<S, XEv>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new RandomDescentMethod<S, XEv>(*eval, *ns, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<S, XEv>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandomDescentMethod<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_RDM_HPP_*/
