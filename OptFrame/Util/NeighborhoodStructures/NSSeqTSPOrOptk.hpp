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

#ifndef OPTFRAME_NSSEQ_TSP_OROPTK_HPP_
#define OPTFRAME_NSSEQ_TSP_OROPTK_HPP_

// Framework includes
#include "../../NSSeq.hpp"

#include "Moves/MoveTSPOrOptk.hpp"
#include "NSIterators/IteratorTSPOrOptk.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveTSPOrOptk<T, ADS, DS >, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, DS, MOVE, P> >
class NSSeqTSPOrOptk : public NSSeq<vector<T> , ADS, DS >
{
   typedef vector<T> Route;

private:
   int k;
   P* p; // has to be the last

public:

   NSSeqTSPOrOptk(int _k, P* _p = NULL)
   : k(_k), p(_p)
   {
   }

   virtual ~NSSeqTSPOrOptk()
   {
   }

   using NSSeq<vector<T> , ADS, DS >::move;
   using NSSeq<vector<T> , ADS, DS >::getIterator;

   Move<Route, ADS, DS >& move(const Route& rep, const ADS&)
   {
      int n = rep.size();

      if(n - k <= 0)
         return *new MOVE(0, 0, k, p);

      int i = rand() % ( n - k + 1 );

      int j = i;
      while(i == j)
         j = rand() % ( n - k + 1 );

      return *new MOVE(i, j, k, p);
   }

   virtual NSIterator<Route, ADS, DS >& getIterator(const Route& r, const ADS&)
   {
      return *new NSITERATOR (r.size(), k, p);
   }

   static string idComponent()
   {
	   stringstream ss;
	   ss << NSSeq<vector<T>, ADS, DS >::idComponent() << ":NSSeqTSPOrOptk";
	   return ss.str();
   }

   virtual string id() const
   {
	   return idComponent();
   }

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( NSSeq<vector<T>, ADS, DS >::compatible(s) );
   }

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqTSPOrOpt{K=" << k << "} with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
