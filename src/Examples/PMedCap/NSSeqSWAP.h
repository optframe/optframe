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

#ifndef PCAP_NSSEQSWAP_HPP_
#define PCAP_NSSEQSWAP_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"

//#include "../../OptFrame/Util/TestMove.hpp"

#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace pmedcap
{

class MoveSWAP : public Move<SolutionPCAP>
{
public:
   int x;
   int med;
   MoveSWAP(int _x, int _med)
   {
      x = _x;
      med = _med;
   }

   virtual ~MoveSWAP()
   {
   }

   bool canBeApplied(const SolutionPCAP& s) override
   {
      return true;
   }

   Move<SolutionPCAP>* apply(SolutionPCAP& s) override
   {
      RepPCAP& rep = s.getR();
      int aux = rep.second[x];
      rep.second[x] = med;
      return new MoveSWAP(x, aux);
   }

   virtual bool operator==(const Move<SolutionPCAP>& _m) const
   {
      const MoveSWAP& m = (const MoveSWAP&)_m;
      return x == m.x;
   }

   void print() const
   {
      cout << "MoveSwapMediana: (" << x << "," << med << ")" << endl;
   }
};

class NSIteratorSWAP : public NSIterator<SolutionPCAP>
{
public:
   PCAPProblemInstance& p;
   int x;
   int mediana;
   NSIteratorSWAP(PCAPProblemInstance& _p)
     : p(_p)
   {
   }

   virtual ~NSIteratorSWAP()
   {
   }

   virtual void first() override
   {
      x = 0;
      mediana = 0;
   }

   virtual void next() override
   {
      mediana++;
      if (mediana == (p.nMedianas - 1)) {
         mediana = 0;
         x++;
      }
   }

   virtual bool isDone() override
   {
      return x == p.nCidades;
   }

   virtual Move<SolutionPCAP>* current() override
   {
      return new MoveSWAP(x, mediana);
   }
};

class NSSeqSWAP : public NSSeq<SolutionPCAP>
{
public:
   PCAPProblemInstance& p;
   RandGen& rg;

   NSSeqSWAP(PCAPProblemInstance& _p, RandGen& _rg)
     : p(_p)
     , rg(_rg)
   {
   }

   virtual ~NSSeqSWAP()
   {
   }

   virtual Move<SolutionPCAP>* randomMove(const SolutionPCAP& s) override
   {
      const RepPCAP& rep = s.getR();
      int cidade = rg.rand(rep.second.size());
      int mediana = rg.rand(rep.first.size());
      return new MoveSWAP(cidade, mediana); // return a random move
   }

   virtual NSIterator<SolutionPCAP>* getIterator(const SolutionPCAP& s) override
   {
      return new NSIteratorSWAP(p); // return an iterator to the neighbors of 'rep'
   }

   virtual void print() const
   {
   }
};

}

#endif /*PCAP_NSSEQSWAP_HPP_*/
