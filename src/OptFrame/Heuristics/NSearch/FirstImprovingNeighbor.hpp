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

#ifndef OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_
#define OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_

#include "../../Evaluator.hpp"
#include "../../NSSeq.hpp"
#include "../../NeighborhoodExploration.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class FirstImprovingNeighbor : public NeighborhoodExploration<XES, XEv>
{
private:
   GeneralEvaluator<XES, XEv, XSH>& eval;
   NSSeq<XES, XEv, XSH>& nsSeq;

public:
   FirstImprovingNeighbor(GeneralEvaluator<XES, XEv>& _eval, NSSeq<XES, XEv, XSH>& _nsSeq)
     : eval(_eval)
     , nsSeq(_nsSeq)
   {
   }

   virtual ~FirstImprovingNeighbor()
   {
   }

   virtual op<RichMove<XES, XEv>> searchMove(const XES& se, const StopCriteria<XEv>& stopCriteria) override
   {
      // gets valid iterator
      uptr<NSIterator<XES, XEv>> it = nsSeq.getIterator(se);
      // TODO: may throw? just break now...
      assert(it);
      // first possible move
      it->first();
      // check if any move exists
      if (it->isDone())
         return nullopt; // no neighbor
                         //
      do {
         // gets possible move
         uptr<Move<XES, XEv, XSH>> move = it->current();
         // check if move is valid
         if (move->canBeApplied(se)) {
            // get cost only if it's improving... otherwise, just nullopt
            op<XEv> mcost = this->isImprovingCost(*move, se);
            if (mcost) {
               // should not apply to solution directly! must return...
               RichMove<XES, XEv> rmove;
               rmove.move = std::move(move);
               rmove.cost = std::move(*mcost);
               rmove.status = SearchStatus::IMPROVEMENT;
               //return make_optional(rmove);
               return std::optional<RichMove<XES, XEv>>(std::move(rmove));
            }
         }
         // get next possible move
         it->next();
      } while (!it->isDone());
      //
      // finished search
      return std::nullopt;
   }

   // returns 'cost' if it's improving, otherwise std::nullopt
   op<XEv> isImprovingCost(Move<XES, XEv>& m, const XSH& cse, bool allowEstimated = false)
   {
      // try to get a cost
      op<XEv> p = m.cost(cse, allowEstimated);
      // if p not null => much faster (using cost)
      if (p) {
         // verify if m is an improving move
         //if (p->isStrictImprovement()) {
         if (eval.isStrictImprovement(*p))
            return p;
         else
            return nullopt;
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         // remove constness! must make sure 'cse' is never changed from its original state
         XSH& se = const_cast<XSH&>(cse);
         XEv& e = se.second;

         // saving previous evaluation
         XEv ev_begin(e);

         // apply move to both XEv and Solution
         uptr<Move<XES, XEv>> rev = eval.applyMoveReevaluate(m, se);

         // compute cost directly on Evaluation
         XEv mcost = ev_begin.diff(se.second);

         // return to original solution
         rev->apply(se);
         e = std::move(ev_begin);

         // check if it is improvement
         if (eval.isStrictImprovement(mcost)) {
            return make_optional(mcost);
         }

         return nullopt;
      }
      return nullopt;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (NeighborhoodExploration<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << NeighborhoodExploration<XES, XEv>::idComponent() << ":FirstImprovingNeighbor";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "FirstImprovingNeighbor: " << nsSeq.toString();
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class FirstImprovingNeighborBuilder : public NeighborhoodExplorationBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~FirstImprovingNeighborBuilder()
   {
   }

   virtual NeighborhoodExploration<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      GeneralEvaluator<XES, XEv>* eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      NSSeq<XES, XEv, XSH>* nsseq;
      hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new FirstImprovingNeighbor<XES, XEv, XSH>(*eval, *nsseq);
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
      return component == FirstImprovingNeighbor<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":FirstImprovingNeighbor";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_*/
