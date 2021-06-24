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

#ifndef PCAP_NSSEQSWAP_HPP_
#define PCAP_NSSEQSWAP_HPP_

// Framework includes
#include <OptFrame/NSSeq.hpp>

//#include "../../OptFrame/Util/TestMove.hpp"

#include <OptFrame/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace pmedcap {

class MoveSWAP : public Move<ESolutionPCAP>
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

   bool canBeApplied(const ESolutionPCAP& s) override
   {
      return true;
   }

   uptr<Move<ESolutionPCAP>> apply(ESolutionPCAP& s) override
   {
      RepPCAP& rep = s.first.getR();
      int aux = rep.second[x];
      rep.second[x] = med;
      return uptr<Move<ESolutionPCAP>>(new MoveSWAP(x, aux));
   }

   virtual bool operator==(const Move<ESolutionPCAP>& _m) const
   {
      const MoveSWAP& m = (const MoveSWAP&)_m;
      return x == m.x;
   }

   void print() const
   {
      cout << "MoveSwapMediana: (" << x << "," << med << ")" << endl;
   }
};

class NSIteratorSWAP : public NSIterator<ESolutionPCAP>
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

   virtual uptr<Move<ESolutionPCAP>> current() override
   {
      return uptr<Move<ESolutionPCAP>>(new MoveSWAP(x, mediana));
   }
};

class NSSeqSWAP : public NSSeq<ESolutionPCAP>
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

   virtual uptr<Move<ESolutionPCAP>> randomMove(const ESolutionPCAP& s) override
   {
      const RepPCAP& rep = s.first.getR();
      int cidade = rg.rand(rep.second.size());
      int mediana = rg.rand(rep.first.size());
      return uptr<Move<ESolutionPCAP>>(new MoveSWAP(cidade, mediana)); // return a random move
   }

   virtual uptr<NSIterator<ESolutionPCAP>> getIterator(const ESolutionPCAP& s) override
   {
      return uptr<NSIterator<ESolutionPCAP>>(new NSIteratorSWAP(p)); // return an iterator to the neighbors of 'rep'
   }

   virtual void print() const
   {
   }
};

}

#endif /*PCAP_NSSEQSWAP_HPP_*/
