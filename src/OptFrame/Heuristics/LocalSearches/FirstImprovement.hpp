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

template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
class FirstImprovement: public LocalSearch<S, XEv>
{
private:
	GeneralEvaluator<S, XEv, XSH>& eval;
	NSSeq<S, XEv, XSH>& nsSeq;

public:

	FirstImprovement(Evaluator<S, XEv>& _eval, NSSeq<S, XEv, XSH>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovement()
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
      //XEv& e = se.second;
		uptr<NSIterator<S, XEv>> it = nsSeq.getIterator(s);
      // TODO: return FAILED result...
      assert(it);
		string bestMoveId = "";
		it->first();

		if (it->isDone())
		{
			return; // return OK!
		}

		do
		{
			uptr<Move<S, XEv, XSH>> move = it->current();

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
				if(this->acceptsImprove(*move, se))
				{
					// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
					//e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

					return;
				}
			}

			it->next();
		}
		while (!it->isDone());

		// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
		//if(bestMoveId != "")
		//	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum
	}



   // used on FirstImprovement
   // Accept and apply move if it improves parameter moveCost
   ///bool acceptsImprove(Move<S, XEv>& m, XSH& se, MoveCost<>* mc = nullptr, bool allowEstimated = false)
   bool acceptsImprove(Move<S, XEv>& m, XSH& se, XEv* mc = nullptr, bool allowEstimated = false)
   {
      // TODO: in the future, consider 'allowEstimated' parameter

      S& s = se.first;
      XEv& e = se.second;

      // initialize MoveCost pointer
      //MoveCost<>* p = nullptr;
      op<XEv> p = nullopt;
      // try to get a cost (should consider estimated moves in the future)
      
      p = m.cost(se, allowEstimated);
      

      // if p not null => much faster (using cost)
      if (p) {
         // verify if m is an improving move
         if (p->isStrictImprovement()) {
            // apply move and get reverse
            uptr<Move<S, XEv>> rev = m.apply(s);
            XEv& eOpt = *p;
            // update XEv with "MoveCost" (now Evaluation represents 'costs')
            //p->update(e);           
            eOpt.update(e);
            return true;
         } else {
            return false;
         }
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         // saving previous evaluation
         XEv ev_begin(e);
         // saving 'outdated' status to avoid inefficient re-evaluations
         //			bool outdated = e.outdated;
         // get original obj function values
         pair<evtype, evtype> e_begin = make_pair(e.getObjFunction(), e.getInfMeasure());
         // get original values for lexicographic part
         vector<pair<evtype, evtype>> alt_begin(e.getAlternativeCosts().size());
         for (unsigned i = 0; i < alt_begin.size(); i++) {
            alt_begin[i].first = e.getAlternativeCosts()[i].first;
            alt_begin[i].second = e.getAlternativeCosts()[i].second;
         }
         // apply move to both XEv and Solution
         uptr<Move<S, XEv>> rev = eval.applyMoveReevaluate(m, se);

         // TODO: check outdated and estimated!
         //MoveCost mcost(e.getObjFunction() - e_begin.first, e.getInfMeasure() - e_begin.second, 1, false, false);
         //Evaluation<> mcost(e.getObjFunction() - e_begin.first, e.getInfMeasure() - e_begin.second, 1, false, false);
         Evaluation<> mcost(e.getObjFunction() - e_begin.first, e.getInfMeasure() - e_begin.second, 1); // no outdated or estimated

         // guarantee that alternative costs have same size
         assert(alt_begin.size() == e.getAlternativeCosts().size());
         
         // compute alternative costs
         for (unsigned i = 0; i < alt_begin.size(); i++)
            mcost.addAlternativeCost(make_pair(e.getAlternativeCosts()[i].first - alt_begin[i].first, e.getAlternativeCosts()[i].second - alt_begin[i].second));

         // check if it is improvement
         if (mcost.isStrictImprovement()) {
            return true;
         }

         // must return to original situation

         // apply reverse move in order to get the original solution back
         //TODO - Vitor, Why apply Move with e is not used???
         //			Even when reevaluate is implemented, It would be hard to design a strategy that is faster than copying previous evaluation
         //==================================================================
         //pair<Move<S, XEv>*, XEv> ini = applyMove(*rev, s);

         // if XEv wasn't 'outdated' before, restore its previous status
         //			if (!outdated)
         //				e.outdated = outdated;

         // go back to original evaluation
         //			e = ini.second;
         //			delete ini.first;

         uptr<Move<S, XEv>> ini = rev->apply(s);
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);
         e = std::move(ev_begin);
         //==================================================================

         return false;
      }
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


template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
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

		NSSeq<S, XEv, XSH>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<S, XEv, XSH>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<S, XEv, XSH>::idComponent(), "neighborhood structure"));

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
