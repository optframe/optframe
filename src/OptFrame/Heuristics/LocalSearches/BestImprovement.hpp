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

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

#include "../../Timer.hpp"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class BestImprovement: public LocalSearch<XES, XEv>
{
private:
	sref<GeneralEvaluator<XES, XEv>> eval;
	sref<NSSeq<XES, XEv, XSH>> nsSeq;

	// logs
	double sum_time;
	int num_calls;

public:

	BestImprovement(sref<GeneralEvaluator<XES, XEv>> _eval, sref<NSSeq<XES, XEv, XSH>> _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
		sum_time = 0.0;
		num_calls = 0;
	}

	virtual ~BestImprovement()
	{
	}

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& sosc) override
	//{
	//	Evaluation<> e = eval.evaluate(s);
	//	exec(s, e, sosc);
	//}

	virtual SearchStatus searchFrom(XSH& se, const StopCriteria<XEv>& sosc) override
	{
      //XSolution& s = se.first;
      //XEv& e = se.second;

      //double timelimit = sosc.timelimit;
      //double target_f = sosc.target_f; 

		if(Component::information)
			cout << "BI::starts for " << nsSeq->toString() << endl;

		num_calls++;
		Timer t;

		// TODO: verify if it's not null
		//uptr<NSIterator<XES, XEv, XSH>> it = nsSeq.getIterator(s);
      uptr<NSIterator<XES, XEv, XSH>> it = nsSeq->getIterator(se);
      //
      if(!it)
         return SearchStatus::FAILED;
      //
		it->first();

		if (it->isDone())
		{
			sum_time += t.inMilliSecs();
			return SearchStatus::NO_REPORT;
		}

		uptr<Move<XES, XEv, XSH>> bestMove = it->current();

		/*if(e.getLocalOptimumStatus(bestMove->id()))
		{
			delete &it;
			delete bestMove;

			sum_time += t.inMilliSecs();
			return;
		}*/

		//MoveCost<>* bestCost = nullptr;
      op<XEv> bestCost = nullopt;

		while (true)
		{
			//while (!bestMove->canBeApplied(s))
         while (!bestMove->canBeApplied(se))
			{
				it->next();
				if (!it->isDone())
				{
					bestMove = it->current();
				}
				else
				{
					sum_time += t.inMilliSecs();
					return SearchStatus::NO_REPORT;
				}
			}

			bestCost = eval->moveCost(*bestMove, se);
			//if (eval.isImprovement(*bestCost))
         //if (bestCost->isImprovingStrict())
         if (eval->isStrictImprovement(*bestCost))
			{
				it->next();
				break;
			}
			else
			{
				it->next();

				if (!it->isDone())
				{
					bestMove = it->current();
				}
				else
				{
					sum_time += t.inMilliSecs();
					return SearchStatus::NO_REPORT;
				}
			}

		}


		//it.next();
		while (!it->isDone())
		{
			uptr<Move<XES, XEv>> move = it->current();
			//if (move->canBeApplied(s))
         if (move->canBeApplied(se))
			{
				///MoveCost<>* cost = eval.moveCost(*move, se);
            op<XEv> cost = eval->moveCost(*move, se);

				//if (eval.betterThan(*cost, *bestCost))
            //if (cost->betterStrict(*bestCost))
            if (eval->betterStrict(*cost, *bestCost))
				{
					/////delete bestMove;
					/////delete bestCost;
               //
					//bestMove = move;
					//bestCost = cost;
               bestMove = std::move(move);
               move = nullptr;
					bestCost = cost; // TODO: std::move?
				}
				else
				{
					//delete move;
					//delete cost;
				}
			}
			else
         {
				//delete move;
         }

			it->next();
		}

		//if (eval.isImprovement(*bestCost))
      //if (bestCost->isImprovingStrict())
      if (eval->isStrictImprovement(*bestCost))
		{
			//cout << "MOVE IS IMPROVEMENT! cost=";
			//bestCost->print();

			if(bestCost->isEstimated())
			{
				// TODO: have to test if bestMove is ACTUALLY an improvement move...
			}

			bestMove->applyUpdate(se);

			eval->reevaluate(se); // updates 'e'
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

		sum_time += t.inMilliSecs();
      return SearchStatus::NO_REPORT;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << "BI";
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
		ss << "BI: " << nsSeq->toString();
		return ss.str();
	}

	virtual string log() const
	{
		stringstream ss;
		ss << sum_time;
		return ss.str();
	}

};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
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
		sptr<GeneralEvaluator<XES, XEv>> eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		if(!scanner.hasNext())
			return nullptr;
		sptr<NSSeq<XES, XEv, XSH>> nsseq;
		hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

		return new BestImprovement<XES, XEv, XSH>(eval, nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BestImprovement<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":BI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BI_HPP_*/
