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

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NSBlockIterator.hpp"
#include "NSFind.hpp"
#include "NSIterator.hpp"

#include <type_traits> // static assert is_same

using namespace std;

namespace optframe {

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NSSeq : public NSFind<XES, XEv, XSH>
{
public:
   /*
   NSSeq()
   {
      static_assert(std::is_same<XEv, decltype(declval<XES>.second)>, "XEv type test over XES");
   }
*/
   virtual ~NSSeq()
   {
   }

   virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;

   virtual uptr<NSIterator<XES, XEv, XSH>> getIterator(const XES&) = 0;

   // experimental! Should create novel NSSeqBlock perhaps
   virtual uptr<NSBlockIterator<XES, XEv>> getBlockIterator(const XES& s)
   {
      uptr<NSIterator<XES, XEv>> it = this->getIterator(s);
      return uptr<NSBlockIterator<XES, XEv>>(new DefaultNSBlockIterator<XES, XEv>(*it));
   }

   // ============= For 'Local Optimum'-based methods =============

   // GET LOCAL OPTIMUM INFORMATION FROM SOLUTION (ADS PREFERED?)
   virtual LOS getLOS(const XES& s)
   {
      return los_unknown;
   }

   // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
   // MAYBE IT's BETTER TO USE ONLY IN ITERATORS! TODO: THINK ABOUT IT...
   //virtual void setLOS(LOS status, S& s)
   //{
   //}

public:
   // =======================================
   // find section (neighborhood exploration)
   // =======================================

private:
   // stateful iterator local variable
   uptr<NSIterator<XES, XEv, XSH>> it;

public:
   // findFirst (from NSSeq): returns the *first* move that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // note that this method is *not stateless* regarding NSSeq class, as a *stateful* iterator variable is locally stored
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findFirst(GeneralEvaluator<XES, XEv, XSH>& gev, XES& se)
   {
      // initializes iterator
      it = this->getIterator(se);
      // finds first valid move
      it->firstValid(se);
      // gets current move (shall we test for isDone()?)
      uptr<Move<XES, XEv, XSH>> pm = it->current();
      // stores temporary raw pointer
      Move<XES, XEv, XSH>* m = pm.get();
      // gets cost for current move
      op<XEv> mvcost = gev.moveCost(*m, se);
      // if no cost, finishes
      if (!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      // loops until finds improving move
      while (!gev.isStrictImprovement(*mvcost)) {
         // if done, finishes
         if (it->isDone())
            return std::make_pair(nullptr, std::nullopt);
         it->nextValid(se);
         // gets current move (shall we test for isDone()?)
         pm = it->current();
         // stores temporary raw pointer
         m = pm.get();
         // gets cost for current move
         mvcost = gev.moveCost(*m, se);
         // if no cost, finishes
         if (!mvcost)
            return std::make_pair(nullptr, std::nullopt);
      }
      // current move should be improving
      return std::make_pair(pm.release(), mvcost);
   }

   // findNext (from NSSeq): returns the *next* move that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // note that this method is *not stateless* regarding NSSeq class, as a *stateful* iterator variable is locally stored
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findNext(GeneralEvaluator<XES, XEv, XSH>& gev, XES& se)
   {
      // checks if iterator is initialized or finished
      if (!it || it->isDone())
         return std::make_pair(nullptr, std::nullopt);
      // finds next valid move
      it->nextValid(se);
      // gets current move (shall we test for isDone()?)
      uptr<Move<XES, XEv, XSH>> pm = it->current();
      // stores temporary raw pointer
      Move<XES, XEv, XSH>* m = pm.get();
      // gets cost for current move
      op<XEv> mvcost = gev.moveCost(*m, se);
      // if no cost, finishes
      if (!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      // loops until finds improving move
      while (!gev.isStrictImprovement(*mvcost)) {
         // if done, finishes
         if (it->isDone())
            return std::make_pair(nullptr, std::nullopt);
         it->nextValid(se);
         // gets current move (shall we test for isDone()?)
         pm = it->current();
         // stores temporary raw pointer
         m = pm.get();
         // gets cost for current move
         mvcost = gev.moveCost(*m, se);
         // if no cost, finishes
         if (!mvcost)
            return std::make_pair(nullptr, std::nullopt);
      }
      // current move should be improving
      return std::make_pair(pm.release(), mvcost);
   }

   // findBest (from NSFind): returns move that greatly improves current solution 'se', according 'gev'
   // NSFind is useful for exponential-sized neighborhoods, without requiring any iterator structure
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findBest(GeneralEvaluator<XES, XEv, XSH>& gev, XES& se) override
   {
      // finds first improving move
      pair<Move<XES, XEv, XSH>*, op<XEv>> mve = this->findFirst(gev, se);
      if (!mve.second)
         return std::make_pair(nullptr, std::nullopt);
      op<XEv> bestCost = std::move(mve.second);
      Move<XES, XEv, XSH>* bestMove = mve.first;
      // iterates while iterator is valid
      while (!it->isDone()) {
         // gets next improving move
         pair<Move<XES, XEv, XSH>*, op<XEv>> mveNext = this->findNext(gev, se);
         // checks if it surpasses existing best move
         if (mve.second && gev.betterStrict(*mve.second, *bestCost)) {
            bestCost = std::move(mve.second);
            bestMove = mve.first;
         }
      }
      return std::make_pair(bestMove, bestCost);
   }

public:
   static string idComponent()
   {
      stringstream ss;
      ss << NSFind<XES, XEv>::idComponent() << ":NSSeq";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual std::string toString() const override
   {
      return id();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (NSFind<XES, XEv>::compatible(s));
   }
};

} // namespace optframe

#ifndef NDEBUG

// General test for NSSeq
// TODO: only if not #include "printable.h"
#include "BaseConcepts.ctest.hpp"
#include "BaseConcepts.hpp"

namespace optframe {

using nsseq_test_base = NSSeq<pair<IsSolution<vector<int>, short>, Evaluation<>>>;

} // using namespace optframe
#endif

#endif /*OPTFRAME_NSSEQ_HPP_*/
