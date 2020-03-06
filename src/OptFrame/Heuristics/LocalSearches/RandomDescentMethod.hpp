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

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class RandomDescentMethod: public LocalSearch<XES, XEv, XES>
{
private:
	GeneralEvaluator<XES, XEv>& evaluator;
	NS<XES, XEv>& ns;
	unsigned int iterMax;

public:

	RandomDescentMethod(GeneralEvaluator<XES, XEv>& _eval, NS<XES, XEv>& _ns, unsigned int _iterMax) :
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

	virtual void searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) override
	{
      //XSolution& s = se.first;
      //XEv& e = se.second;
		Timer tNow;

		unsigned int iter = 0;

      // TODO: de-referentiation of 'target_f' WILL crash, if not provided!! removing 'target_f'
		while ((iter < iterMax) && (tNow.now() < stopCriteria.timelimit)) //&& (evaluator.betterThan(*stopCriteria.target_f, se)))
		{
			//uptr<Move<XES, XEv>> move = ns.randomMove(s);
         uptr<Move<XES, XEv>> move = ns.randomMove(se);

			op<Evaluation<>> cost = nullopt;

			//if (move && move->canBeApplied(s))
         if (move && move->canBeApplied(se))
			{
				cost = evaluator.moveCost(*move, se);
			}
			else
			{
				iter++;
				continue;
			}

			iter++;

			//if (cost && evaluator.isImprovement(*cost))
         if (cost && cost->isImprovingStrict())
			{
				move->applyUpdate(se);
				evaluator.reevaluate(se);
				iter = 0;
			}
		}
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << ":RDM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class RandomDescentMethodBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~RandomDescentMethodBuilder()
	{
	}

	virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		GeneralEvaluator<XES, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<XES, XEv>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new RandomDescentMethod<XES, XEv>(*eval, *ns, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<XES, XEv>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandomDescentMethod<XES, XEv>::idComponent();
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
