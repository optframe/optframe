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

#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

#include "NSEnumIterator.hpp"

using namespace std;

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class NSEnum : public NSSeq<XES, XEv, XSH>
{

protected:
   RandGen& rg;

public:
   NSEnum(RandGen& _rg)
     : rg(_rg)
   {}

   virtual ~NSEnum()
   {
   }

   virtual uptr<Move<XES, XEv>> randomMove(const XES&)
   {
      unsigned int x = rg.rand(size());
      return indexMove(x);
   }

   virtual uptr<NSIterator<XES, XEv>> getIterator(const XES&)
   {
      return uptr<NSIterator<XES, XEv>>(new NSEnumIterator<XES, XEv>(*this));
   }

   virtual uptr<Move<XES, XEv>> indexMove(unsigned int index) = 0;

   virtual unsigned int size() const = 0;

public:
   // findFrom: returns the *next* move starting from index k (inclusive), that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findFrom(unsigned int& k, GeneralEvaluator<XES>& gev, XES& se)
   {
      // iterates from k
      for (unsigned xk = k; xk < this->size(); xk++) {
         // gets variable index 'xk'
         uptr<Move<XES, XEv>> pm = this->indexMove(xk);
         if (!pm->canBeApplied(se))
            continue;
         Move<XES, XEv, XSH>* m = pm.get();
         op<XEv> mvcost = gev.moveCost(*m, se);
         if (gev.isStrictImprovement(*mvcost)) {
            k = xk; // updates k
            return std::make_pair(m, mvcost);
         }
      }
      k = this->size(); // updates k
      // no good move
      return std::make_pair(nullptr, std::nullopt);
   }

   // findBest: returns move that greatly improves current solution 'se', according 'gev'
   // iterates using findFrom: 0..size-1
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findBest(GeneralEvaluator<XES>& gev, XES& se)
   {
      // starts count iterator
      unsigned int k = 0;
      // best move
      op<XEv> bestCost = std::nullopt;
      Move<XES, XEv, XSH>* bestMove = nullptr;
      // stops at maximum (or return)
      while (k < this->size()) {
         // gets index 'k'
         pair<Move<XES, XEv, XSH>*, op<XEv>> mve = findFrom(k, gev, se);
         // if not good, process is finished: returns existing best
         if (!mve.second)
            return std::make_pair(bestMove, bestCost);
         // if improvement exists, try to improve best
         if ((!bestCost) || gev.betterStrict(*mve.second, *bestCost)) {
            bestMove = mve.first;
            bestCost = std::move(mve.second);
         }
      }
      // finished: returns best available
      return std::make_pair(bestMove, bestCost);
   }

public:
   static string idComponent()
   {
      stringstream ss;
      ss << NSSeq<XES, XEv, XSH>::idComponent() << ":NSEnum";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (NSSeq<XES, XEv, XSH>::compatible(s));
   }
};

}

#endif /*OPTFRAME_NSENUM_HPP_*/
