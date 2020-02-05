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

#ifndef OPTFRAME_BI_LOS_HPP_
#define OPTFRAME_BI_LOS_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

#include "../../Timer.hpp"

namespace optframe
{

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BestImprovementLOS: public LocalSearch<R, ADS, S>
{
private:
	Evaluator<S, XEv>& eval;
	NSSeq<S, XEv>& nsSeq;

public:

	BestImprovementLOS(Evaluator<S, XEv>& _eval, NSSeq<S, XEv>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~BestImprovementLOS()
	{
	}

	virtual void exec(S& s, SOSC& sosc) override
	{
		Evaluation<> e = eval.evaluateSolution(s);

		exec(s, e, sosc);
	}

	virtual void exec(S& s, Evaluation<>& e, SOSC& sosc) override
	{
		Timer t;

		//TODO: use block iterator and manage each partial local optima discovered

		NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(s);

		cout << "TODO: BestImprovementLOS UNIMPLEMENTED!" << endl;

		return;

		NSIterator<S, XEv>& it = *nsSeq.getIterator(s);

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		Move<S, XEv>* bestMove = it.current();

		/*if(e.getLocalOptimumStatus(bestMove->id()))
		{
			delete &it;
			delete bestMove;

			sum_time += t.inMilliSecs();
			return;
		}*/

		MoveCost<>* bestCost = nullptr;

		while (true)
		{
			while (!bestMove->canBeApplied(s))
			{
				delete bestMove;
				it.next();
				if (!it.isDone())
				{
					bestMove = it.current();
				}
				else
				{
					delete &it;
					return;
				}
			}

			bestCost = eval.moveCost(e, *bestMove, s);
			if (eval.isImprovement(*bestCost))
			{
				it.next();
				break;
			}
			else
			{
				delete bestCost;
				delete bestMove;
				it.next();

				if (!it.isDone())
				{
					bestMove = it.current();
				}
				else
				{
					delete &it;
					return;
				}
			}

		}


		//MoveCost<>* bestCost = &eval.moveCost(e, *bestMove, s);
		//it.next();
		while (!it.isDone())
		{
			Move<S, XEv>* move = it.current();
			if (move->canBeApplied(s))
			{
            bool mayEstimate = false;
				MoveCost<>* cost = eval.moveCost(e, *move, s, mayEstimate);

				if (eval.betterThan(*cost, *bestCost))
				{
					delete bestMove;
					delete bestCost;
					bestMove = move;
					bestCost = cost;
				}
				else
				{
					delete move;
					delete cost;
				}
			}
			else
				delete move;

			it.next();
		}

		if (eval.isImprovement(*bestCost))
		{
			//cout << "MOVE IS IMPROVEMENT! cost=";
			//bestCost->print();

			if(bestCost->isEstimated())
			{
				// TODO: have to test if bestMove is ACTUALLY an improvement move...
			}

			Component::safe_delete(bestMove->applyUpdate(e, s));

			eval.reevaluateSolution(e, s); // updates 'e'
			//e.setLocalOptimumStatus(bestMove->id(), false); //set NS 'id' out of Local Optimum
		}
		else{
			//bestMove->updateNeighStatus(s.getADS());
			//e.setLocalOptimumStatus(bestMove->id(), true); //set NS 'id' on Local Optimum
		}
		//cout << "#" << num_calls << " out_bi:";
		//bestMove->print();
	    //nsSeq.print();
		//e.print();

		delete bestCost;
		delete bestMove;
		delete &it;
	}





	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, S>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, S>::idComponent() << "BI_LOS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << toString() << endl;
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "BI_LOS: " << nsSeq.toString();
		return ss.str();
	}

};


template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BestImprovementBuilder : public LocalSearchBuilder<R, ADS, S>
{
public:
	virtual ~BestImprovementBuilder()
	{
	}

	virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		if(!scanner.hasNext())
			return nullptr;
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		if(!scanner.hasNext())
			return nullptr;
		NSSeq<S, XEv>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BestImprovementLOS<R, ADS, S>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<S, XEv>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BestImprovementLOS<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, S>::idComponent() << ":BI_LOS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BI_LOS_HPP_*/
