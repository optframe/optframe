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

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqTSPOrOptk :
      public NSSeq<vector<T> , ADS, M>
{
   typedef vector<T> Route;
   int k;

public:

   NSSeqTSPOrOptk(int _k) :
      k(_k)
   {
   }

   virtual ~NSSeqTSPOrOptk()
   {
   }

   using NSSeq<vector<T> , ADS, M>::move;
   using NSSeq<vector<T> , ADS, M>::getIterator;

   Move<Route, ADS, M>& move(const Route& rep)
   {
      int n = rep.size();

      if(n - k <= 0)
         return *new MoveTSPOrOptk<T, ADS, M> (0, 0, k);

      int i = rand() % ( n - k + 1 );

      int j = i;
      while(i == j)
         j = rand() % ( n - k + 1 );

      return *new MoveTSPOrOptk<T, ADS, M> (i, j, k);
   }

   virtual NSIterator<Route, ADS, M>& getIterator(const Route& r)
   {
      return *new NSIteratorTSPOrOptk<T, ADS, M> (r.size(), k);
   }

   static string idComponent()
   {
	   stringstream ss;
	   ss << NSSeq<vector<T>, ADS, M>::idComponent() << ":NSSeqTSPOrOptk";
	   return ss.str();
   }

   virtual string id() const
   {
	   return idComponent();
   }

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( NSSeq<vector<T>, ADS, M>::compatible(s) );
   }


   virtual void print() const
   {
      cout << "NSSeqTSPOrOpt{K=" << k << "}" << endl;
   }
};

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
