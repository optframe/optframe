// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class FirstImprovement : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv, XSH>> eval;
   sref<NSSeq<XES, XEv, XSH>> nsSeq;

public:
   FirstImprovement(sref<GeneralEvaluator<XES, XEv>> _eval, sref<NSSeq<XES, XEv, XSH>> _nsSeq)
     : eval(_eval)
     , nsSeq(_nsSeq)
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

   virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) override
   {
      if (Component::verbose) {
         std::cout << "FI: searchFrom begins" << std::endl;
         std::cout << eval->idGE() << std::endl;
      }
      //XSolution& s = se.first;
      //XEv& e = se.second;

      if (Component::verbose)
         std::cout << "FI: getIterator" << std::endl;
      uptr<NSIterator<XES, XEv>> it = nsSeq->getIterator(se);
      //
      if (!it) {
         if (Component::warning)
            std::cout << "FI(WARNING): getIterator FAILED!" << std::endl;
         return SearchStatus::FAILED;
      }
      //
      string bestMoveId = "";
      if (Component::verbose)
         std::cout << "FI: it->first()" << std::endl;
      it->first();

      if (it->isDone()) {
         if (Component::warning)
            std::cout << "FI(WARN): empty neighborhood!" << std::endl;
         return SearchStatus::NO_REPORT;
      }

      do {
         if (Component::verbose)
            std::cout << "FI: it->current()" << std::endl;
         uptr<Move<XES, XEv, XSH>> move = it->current();

         if (!move) {
            if (Component::warning)
               std::cout << "FI(WARNING): it->current() FAILED!" << std::endl;
            return SearchStatus::FAILED;
         }

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

         if (Component::verbose)
            std::cout << "FI: move->canBeApplied()" << std::endl;
         if (move->canBeApplied(se)) {
            if (this->acceptsImprove(*move, se)) {
               // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
               //e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

               return SearchStatus::IMPROVEMENT;
            }
         }

         if (Component::verbose)
            std::cout << "FI: it->next()" << std::endl;
         it->next();
      } while (!it->isDone());

      // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
      //if(bestMoveId != "")
      //	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum
      return SearchStatus::NO_REPORT;
   }

   // used on FirstImprovement
   // Accept and apply move if it improves parameter moveCost
   ///bool acceptsImprove(Move<XES, XEv>& m, XSH& se, MoveCost<>* mc = nullptr, bool allowEstimated = false)
   bool acceptsImprove(Move<XES, XEv>& m, XSH& se, bool allowEstimated = false)
   {
      if (Component::verbose)
         std::cout << "FI: begin acceptsImprove()" << std::endl;
      //XSolution& s = se.first;
      XEv& e = se.second;

      // try to get a cost
      op<XEv> p = m.cost(se, allowEstimated);

      // if p not null => much faster (using cost)
      if (p) {
         // verify if m is an improving move
         //if (p->isStrictImprovement()) {
         if (eval->isStrictImprovement(*p)) {

            // apply move and get reverse
            uptr<Move<XES, XEv>> rev = m.apply(se);
            // update value using calculated cost
            p->update(e);
            return true;
         } else {
            return false;
         }
      } else {
         if (Component::verbose)
            std::cout << "FI: no cost" << std::endl;
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         // saving previous evaluation
         XEv ev_begin(e);

         // apply move to both XEv and Solution
         uptr<Move<XES, XEv>> rev = eval->applyMoveReevaluate(m, se);

         // compute cost directly on Evaluation
         XEv mcost = ev_begin.diff(se.second);

         if (Component::verbose)
            std::cout << "FI: isStrictImprovement?" << std::endl;
         // check if it is improvement
         if (eval->isStrictImprovement(mcost)) {
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

         if (Component::verbose)
            std::cout << "FI: No improvement. Will reverse." << std::endl;

         uptr<Move<XES, XEv>> ini = rev->apply(se);
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);
         e = std::move(ev_begin);
         //==================================================================

         if (Component::verbose)
            std::cout << "FI: ends acceptsImprove()" << std::endl;

         return false;
      }
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":FI";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "FI: " << nsSeq->toString();
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class FirstImprovementBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~FirstImprovementBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NSSeq<XES, XEv, XSH>> nsseq;
      hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new FirstImprovement<XES, XEv, XSH>(eval, nsseq);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == FirstImprovement<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":FI";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_FI_HPP_*/
