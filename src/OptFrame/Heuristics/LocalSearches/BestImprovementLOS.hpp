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

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class BestImprovementLOS: public LocalSearch<S, XEv, XSH>
{
private:
	Evaluator<XES, XEv>& eval;
	NSSeq<XES, XEv, XSH>& nsSeq;

public:

	BestImprovementLOS(Evaluator<XES, XEv>& _eval, NSSeq<XES, XEv, XSH>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~BestImprovementLOS()
	{
	}

	virtual void exec(S& s, const StopCriteria<XEv>& sosc) override
	{
		Evaluation<> e = eval.evaluate(s);

		exec(s, e, sosc);
	}

	virtual void exec(S& s, Evaluation<>& e, const StopCriteria<XEv>& sosc) override
	{
		Timer t;

		//TODO: use block iterator and manage each partial local optima discovered

		//NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(s);
      NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(se);

		cout << "TODO: BestImprovementLOS UNIMPLEMENTED!" << endl;

		return;

		//NSIterator<S, XEv>& it = *nsSeq.getIterator(s);
      NSIterator<S, XEv>& it = *nsSeq.getIterator(se);

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
			//while (!bestMove->canBeApplied(s))
         while (!bestMove->canBeApplied(se))
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

			bestCost = eval.moveCost(*bestMove, se);
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


		//it.next();
		while (!it.isDone())
		{
			Move<S, XEv>* move = it.current();
			if (move->canBeApplied(s))
			{
            bool mayEstimate = false;
				MoveCost<>* cost = eval.moveCost(*move, se, mayEstimate);

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

			eval.reevaluate(e, s); // updates 'e'
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
		return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << "BI_LOS";
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


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BestImprovementBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~BestImprovementBuilder()
	{
	}

	virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		if(!scanner.hasNext())
			return nullptr;
		Evaluator<XES, XEv>* eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		if(!scanner.hasNext())
			return nullptr;
		NSSeq<XES, XEv, XSH>* nsseq;
		hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

		return new BestImprovementLOS<S, XEv>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BestImprovementLOS<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":BI_LOS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BI_LOS_HPP_*/
