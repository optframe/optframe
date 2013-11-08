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

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

#include "../../Timer.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BestImprovement: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& eval;
	NSSeq<R, ADS, DS>& nsSeq;

	// logs
	double sum_time;
	int num_calls;

public:

	BestImprovement(Evaluator<R, ADS, DS>& _eval, NSSeq<R, ADS, DS>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
		sum_time = 0.0;
		num_calls = 0;
	}

	virtual ~BestImprovement()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = eval.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
		num_calls++;
		Timer t;

		NSIterator<R, ADS, DS>& it = nsSeq.getIterator(s);

		it.first();

		if (it.isDone())
		{
			delete &it;
			sum_time += t.inMilliSecs();
			return;
		}

		Move<R, ADS, DS>* bestMove = &it.current();

		/*if(e.getLocalOptimumStatus(bestMove->id()) == true)
		{
			delete &it;
			delete bestMove;

			sum_time += t.inMilliSecs();
			return;
		}*/

		MoveCost* bestCost = NULL;

		while (true)
		{
			while (!bestMove->canBeApplied(s))
			{
				delete bestMove;
				it.next();
				if (!it.isDone())
				{
					bestMove = &it.current();
				}
				else
				{
					delete &it;

					sum_time += t.inMilliSecs();
					return;
				}
			}

			bestCost = &eval.moveCost(e, *bestMove, s);
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
					bestMove = &it.current();
				}
				else
				{
					delete &it;

					sum_time += t.inMilliSecs();
					return;
				}
			}

		}


		//MoveCost* bestCost = &eval.moveCost(e, *bestMove, s);
		//it.next();
		while (!it.isDone())
		{
			Move<R, ADS, DS>* move = &it.current();
			if (move->canBeApplied(s))
			{
				MoveCost* cost = &eval.moveCost(e, *move, s);

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

			delete &bestMove->apply(e, s);

			eval.evaluate(e, s); // updates 'e'
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
		sum_time += t.inMilliSecs();

	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "BI";
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
		ss << "BI: " << nsSeq.toString();
		return ss.str();
	}

	virtual string log() const
	{
		stringstream ss;
		ss << sum_time;
		return ss.str();
	}

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BestImprovementBuilder : public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~BestImprovementBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<R, ADS, DS>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BestImprovement<R, ADS, DS>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<R, ADS, DS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BestImprovement<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "BI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BI_HPP_*/
