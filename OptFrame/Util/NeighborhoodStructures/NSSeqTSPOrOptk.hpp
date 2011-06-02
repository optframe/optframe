// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

template< class T, class M = OPTFRAME_DEFAULT_MEMORY >
class NSSeqTSPOrOptk :
      public NSSeq<vector<T> , M>
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

   using NSSeq<vector<T> , M>::move;
   using NSSeq<vector<T> , M>::getIterator;

   Move<Route, M>& move(const Route& rep)
   {
      int n = rep.size();

      if(n - k <= 0)
         return *new MoveTSPOrOptk<T, M> (0, 0, k);

      int i = rand() % ( n - k + 1 );

      int j = i;
      while(i == j)
         j = rand() % ( n - k + 1 );

      return *new MoveTSPOrOptk<T, M> (i, j, k);
   }

   virtual NSIterator<Route, M>& getIterator(const Route& r)
   {
      return *new NSIteratorTSPOrOptk<T, M> (r.size(), k);
   }

   virtual void print()
   {
      cout << "NSSeqTSPOrOpt{K=" << k << "}" << endl;
   }
};

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
