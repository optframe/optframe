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

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class FirstImprovement: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& eval;
	NSSeq<R, ADS>& nsSeq;

public:

	FirstImprovement(Evaluator<R, ADS>& _eval, NSSeq<R, ADS>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovement()
	{
	}

	virtual void exec(Solution<R, ADS>& s, SOSC& stopCriteria)
	{
		Evaluation e = eval.evaluateSolution(s);
		exec(s, e, stopCriteria);
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria)
	{
		NSIterator<R, ADS>& it = *nsSeq.getIteratorSolution(s);
		string bestMoveId = "";
		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<R, ADS>* move = it.current();

			// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
			/*
			if(e.getLocalOptimumStatus(move->id()))
			{
				delete &it;
				delete move;
				return;
			}
			*/

//			bestMoveId = move->id();

			if (move->canBeAppliedToSolution(s))
			{
				if(eval.acceptsImprove(*move,s,e))
				{
					delete move;
					delete &it;
					// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
					//e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

					return;
				}
			}

			delete move;

			it.next();
		}
		while (!it.isDone());

		// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
		//if(bestMoveId != "")
		//	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum

		delete &it;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":FI";
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


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class FirstImprovementBuilder : public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~FirstImprovementBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<R, ADS>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<R, ADS>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<R, ADS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovement<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << ":FI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_FI_HPP_*/
