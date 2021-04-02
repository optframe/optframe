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

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NSFind.hpp"
#include "NSIterator.hpp"
#include "NSBlockIterator.hpp"

#include <type_traits> // static assert is_same

using namespace std;

namespace optframe
{

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NSSeq: public NSFind<XES, XEv, XSH>
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

   // findFirst: returns the *first* move that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // note that this method is *not stateless* regarding NSSeq class, as a *stateful* iterator variable is locally stored
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findFirst(GeneralEvaluator<XES>& gev, XES& se)
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
      if(!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      // loops until finds improving move
      while(!gev.isStrictImprovement(*mvcost))
      {
         // if done, finishes
         if(it->isDone())
            return std::make_pair(nullptr, std::nullopt);
         it->nextValid(se);
         // gets current move (shall we test for isDone()?)
         pm = it->current();
         // stores temporary raw pointer
         m = pm.get(); 
         // gets cost for current move
         mvcost = gev.moveCost(*m, se);
         // if no cost, finishes
         if(!mvcost)
            return std::make_pair(nullptr, std::nullopt);
      }
      // current move should be improving
      return std::make_pair(pm.release(), mvcost);
   }

   // findNext: returns the *next* move that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // note that this method is *not stateless* regarding NSSeq class, as a *stateful* iterator variable is locally stored
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findNext(GeneralEvaluator<XES>& gev, XES& se)
   {
      // checks if iterator is initialized or finished
      if(!it || it->isDone())
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
      if(!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      // loops until finds improving move
      while(!gev.isStrictImprovement(*mvcost))
      {
         // if done, finishes
         if(it->isDone())
            return std::make_pair(nullptr, std::nullopt);
         it->nextValid(se);
         // gets current move (shall we test for isDone()?)
         pm = it->current();
         // stores temporary raw pointer
         m = pm.get(); 
         // gets cost for current move
         mvcost = gev.moveCost(*m, se);
         // if no cost, finishes
         if(!mvcost)
            return std::make_pair(nullptr, std::nullopt);
      }
      // current move should be improving
      return std::make_pair(pm.release(), mvcost);
   }

   // findBest: returns move that greatly improves current solution 'se', according 'gev'
   // NSFind is useful for exponential-sized neighborhoods, without requiring any iterator structure
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findBest(GeneralEvaluator<XES>& gev, XES& se)
   {
      // finds first improving move
      pair< Move<XES, XEv, XSH>*, op<XEv> > mve = this->findFirst(gev, se);
      if(!mve.second)
         return std::make_pair(nullptr, std::nullopt);
      op<XEv> bestCost = std::move(mve.second);
      Move<XES, XEv, XSH>* bestMove = mve.first;
      // iterates while iterator is valid
      while(!it->isDone())
      {
         // gets next improving move
         pair< Move<XES, XEv, XSH>*, op<XEv> > mveNext = this->findNext(gev, se);
         // checks if it surpasses existing best move 
         if(mve.second && gev.betterStrict(*mve.second, *bestCost))
         {
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

    virtual bool compatible(string s)
    {
        return (s == idComponent()) || (NSFind<XES, XEv>::compatible(s));
    }
};

#ifndef NDEBUG

// General test for NSSeq
// TODO: only if not #include "printable.h"
//using nsseq_test_base = NSSeq<pair<IsSolution<vector<int>, short> , Evaluation<> >>;

#endif

} // using namespace optframe

#endif /*OPTFRAME_NSSEQ_HPP_*/
