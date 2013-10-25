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

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FirstImprovement: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& eval;
	NSSeq<R, ADS, DS>& nsSeq;

public:

	FirstImprovement(Evaluator<R, ADS, DS>& _eval, NSSeq<R, ADS, DS>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovement()
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
		NSIterator<R, ADS, DS>& it = nsSeq.getIterator(s);
		string bestMoveId = "";
		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<R, ADS, DS>* move = &it.current();

			if(e.getLocalOptimumStatus(move->id()) == true)
			{
				delete &it;
				delete move;
				return;
			}

			bestMoveId = move->id();

			if (move->canBeApplied(s))
			{
				MoveCost* eCost = &eval.moveCost(e, *move, s); // estimated cost

				if(eval.isImprovement(*eCost))
				{
					if(eCost->isEstimated())
					{
						//double cost = eval.moveCost(e, *move, s); // real cost
						// TODO: find a real cost value...
					}

					if(eval.isImprovement(*eCost))
					{
						delete eCost;

						delete &move->apply(e, s);
						delete move;

						delete &it;

						eval.evaluate(e, s); // updates 'e'

						e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

						return;
					}

				}

				delete eCost;
			}

			delete move;

			it.next();
		}
		while (!it.isDone());

		if(bestMoveId != "")
			e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum

		delete &it;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "FI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "FI: " << nsSeq.toString();
		return ss.str();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FirstImprovementBuilder : public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~FirstImprovementBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<R, ADS, DS>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<R, ADS, DS>(*eval, *nsseq);
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
		return component == FirstImprovement<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "FI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_FI_HPP_*/
