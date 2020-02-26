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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class FirstImprovement: public LocalSearch<S, XEv>
{
private:
	Evaluator<S, XEv>& eval;
	NSSeq<S, XEv>& nsSeq;

public:

	FirstImprovement(Evaluator<S, XEv>& _eval, NSSeq<S, XEv>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovement()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, SOSC<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(pair<S, XEv>& se, SOSC<XEv>& stopCriteria) override
	{
      S& s = se.first;
      XEv& e = se.second;
		NSIterator<S, XEv>& it = *nsSeq.getIterator(s);
		string bestMoveId = "";
		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<S, XEv>* move = it.current();

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

			if (move->canBeApplied(s))
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
		return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<S, XEv>::idComponent() << ":FI";
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


template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class FirstImprovementBuilder : public LocalSearchBuilder<S, XEv, X2ES>
{
public:
	virtual ~FirstImprovementBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<S, XEv>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<S, XEv>(*eval, *nsseq);
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
		return component == FirstImprovement<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":FI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_FI_HPP_*/
