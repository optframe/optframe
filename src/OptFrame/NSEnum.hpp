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

#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

#include "NSEnumIterator.hpp"

using namespace std;

namespace optframe
{


template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class NSEnum: public NSSeq<XES, XEv, XSH>
{

protected:
	RandGen& rg;

public:

	NSEnum(RandGen& _rg):rg(_rg)
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
		return uptr<NSIterator<XES, XEv>>(new NSEnumIterator<XES, XEv> (*this));
	}

	virtual uptr<Move<XES, XEv>> indexMove(unsigned int index) = 0;

	virtual unsigned int size() const = 0;

public:

   // findFrom: returns the *next* move starting from index k (inclusive), that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findFrom(unsigned int& k, GeneralEvaluator<XES>& gev, XES& se)
   {
      // iterates from k
      for(unsigned xk = k; xk < this->size(); xk++)
      {
         // gets variable index 'xk'
         uptr<Move<XES, XEv>> pm = this->indexMove(xk);
         if(!pm->canBeApplied(se))
            continue;
         Move<XES, XEv, XSH>* m = pm.get(); 
         op<XEv> mvcost = gev.moveCost(*m, se);
         if(gev.isStrictImprovement(*mvcost)) {
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
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findBest(GeneralEvaluator<XES>& gev, XES& se)
   {
      // starts count iterator
      unsigned int k = 0;
      // best move
      op<XEv> bestCost = std::nullopt;
      Move<XES, XEv, XSH>* bestMove = nullptr;
      // stops at maximum (or return)
      while(k < this->size())
      {
         // gets index 'k'
         pair< Move<XES, XEv, XSH>*, op<XEv> > mve = findFrom(k, gev, se);
         // if not good, process is finished: returns existing best
         if(!mve.second)
            return std::make_pair(bestMove, bestCost);
         // if improvement exists, try to improve best
         if( (!bestCost) || gev.betterStrict(*mve.second, *bestCost) )
         {
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
	   return ( s == idComponent() ) || ( NSSeq<XES, XEv, XSH>::compatible(s) );
   }
};

}

#endif /*OPTFRAME_NSENUM_HPP_*/
