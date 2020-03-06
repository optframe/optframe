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

#ifndef TSP_NSENUMSwap_HPP_
#define TSP_NSENUMSwap_HPP_

// Framework includes
#include "../../OptFrame/Move.hpp"
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "../../OptFrame/RandGen.hpp"
#include "ProblemInstance.h"
#include "Solution.h"

//#define MOV_Swap_DEBUG

using namespace std;

//============================================================================
//                           Swap MOVE
//============================================================================

namespace TSP {

class MoveSwap : public Move<ESolutionTSP>
{
private:
   int c1, c2;
   ProblemInstance& tsp;

public:
   MoveSwap(int c1, int c2, ProblemInstance& _tsp)
     : tsp(_tsp)
   {
      this->c1 = c1;
      this->c2 = c2;

      // Put the rest of your code here
   }

   bool canBeApplied(const ESolutionTSP& s) override
   {
      // If there are some move "MoveSwap" that can't be applied, implement this method

      return true;
   }

   uptr<Move<ESolutionTSP>> apply(ESolutionTSP& s) override
   {
      RepTSP& rep = s.first.getR();
      // Specify how the move "MoveSwap" will be applied

      int aux = rep.at(c1);
      rep[c1] = rep[c2];
      rep[c2] = aux;

      // return the reverse move
      return uptr<Move<ESolutionTSP>>(new MoveSwap(c2, c1, tsp));
   }

   uptr<Move<ESolutionTSP>> applyUpdate(ESolutionTSP& se) override
   {
      SolutionTSP& s = se.first;
      Evaluation<>& e = se.second;
      RepTSP& rep = s.getR();
      int k1, k2;

      if (c1 < c2) {
         k1 = c1;
         k2 = c2;
      } else {
         k1 = c2;
         k2 = c1;
      }

      // before k2 and k1
      int bk1 = k1 - 1;
      int bk2 = k2 - 1;
      // after k2 and k1
      int ak1 = k1 + 1;
      int ak2 = k2 + 1;

      if (k1 == 0)
         bk1 = rep.size() - 1;
      if (k2 == ((int)rep.size()) - 1)
         ak2 = 0;

      double f = 0;

      if (k2 - k1 == 1) // special case, cities are near
      {
         f -= (*tsp.dist)(rep[bk1], rep[k1]);
         f -= (*tsp.dist)(rep[k1], rep[k2]);
         f -= (*tsp.dist)(rep[k2], rep[ak2]);
      } else {
         f -= (*tsp.dist)(rep[bk1], rep[k1]);
         f -= (*tsp.dist)(rep[k1], rep[ak1]);
         f -= (*tsp.dist)(rep[bk2], rep[k2]);
         f -= (*tsp.dist)(rep[k2], rep[ak2]);
      }

      uptr<Move<ESolutionTSP>> rev = apply(se);

      if (k2 - k1 == 1) // special case, cities are near
      {
         f += (*tsp.dist)(rep[bk1], rep[k1]);
         f += (*tsp.dist)(rep[k1], rep[k2]);
         f += (*tsp.dist)(rep[k2], rep[ak2]);
      } else {
         f += (*tsp.dist)(rep[bk1], rep[k1]);
         f += (*tsp.dist)(rep[k1], rep[ak1]);
         f += (*tsp.dist)(rep[bk2], rep[k2]);
         f += (*tsp.dist)(rep[k2], rep[ak2]);
      }

      e.setObjFunction(e.getObjFunction() + f);
      e.outdated = false;

      return rev;
   }

   op<EvaluationTSP> cost(const pair<SolutionTSP, Evaluation<>>& se, bool allowEstimated) override
   {
      const SolutionTSP& s = se.first;
      const RepTSP& rep = s.getR();
      int k1, k2;

      if (c1 < c2) {
         k1 = c1;
         k2 = c2;
      } else {
         k1 = c2;
         k2 = c1;
      }

      //cout << "Swap k1=" << k1 << "(" << rep[k1] << ")" << " k2=" << k2 << "(" << rep[k2] << ")" << endl;

      // before k2 and k1
      int bk1 = k1 - 1;
      int bk2 = k2 - 1;
      // after k2 and k1
      int ak1 = k1 + 1;
      int ak2 = k2 + 1;

      if (k1 == 0)
         bk1 = ((int)rep.size()) - 1;
      if (k2 == ((int)rep.size()) - 1)
         ak2 = 0;

      double f = 0;

      if (k2 - k1 == 1) // special case, cities are near (in fact, a 3-opt case... TODO remove this)
      {
         f -= (*tsp.dist)(rep[bk1], rep[k1]);
         f -= (*tsp.dist)(rep[k1], rep[k2]);
         f -= (*tsp.dist)(rep[k2], rep[ak2]);

         f += (*tsp.dist)(rep[bk1], rep[k2]);
         f += (*tsp.dist)(rep[k2], rep[k1]);
         f += (*tsp.dist)(rep[k1], rep[ak2]);
      } else if ((k1 == 0) && (k2 == ((int)rep.size()) - 1)) // special case, extreme points
      {
         f -= (*tsp.dist)(rep[bk2], rep[k2]);
         f -= (*tsp.dist)(rep[k2], rep[k1]);
         f -= (*tsp.dist)(rep[k1], rep[ak1]);

         f += (*tsp.dist)(rep[bk2], rep[k1]);
         f += (*tsp.dist)(rep[k1], rep[k2]);
         f += (*tsp.dist)(rep[k2], rep[ak1]);
      } else {
         f -= (*tsp.dist)(rep[bk1], rep[k1]);
         f -= (*tsp.dist)(rep[k1], rep[ak1]);
         f -= (*tsp.dist)(rep[bk2], rep[k2]);
         f -= (*tsp.dist)(rep[k2], rep[ak2]);

         f += (*tsp.dist)(rep[bk1], rep[k2]);
         f += (*tsp.dist)(rep[k2], rep[ak1]);
         f += (*tsp.dist)(rep[bk2], rep[k1]);
         f += (*tsp.dist)(rep[k1], rep[ak2]);
      }

      //return new MoveCost<>(f, 0);
      return make_optional(Evaluation<>(f,0));
   }

   void print() const
   {
      cout << "MoveSwap between " << c1 << " and " << c2 << endl;
   }

   virtual bool operator==(const Move<ESolutionTSP>& _m) const
   {
      const MoveSwap& m = (const MoveSwap&)_m; // You can only compare if types are equal

      if ((c1 == m.c1 && c2 == m.c2) || (c1 == m.c2 && c2 == m.c1))
         return true;
      else
         return false;
   }
};

//============================================================================
//                  Swap Neighborhood Structure
//============================================================================

class NSEnumSwap : public NSEnum<ESolutionTSP>
{
private:
   ProblemInstance* pI;
   int n;

   // Your private vars

public:

   NSEnumSwap(ProblemInstance* pI, RandGen& _rg)
     : NSEnum<ESolutionTSP>(_rg)
   {
      this->pI = pI;
      this->n = pI->n;
   }

   // given index, returns (i,j), with 0 < i < j < n-1
   virtual uptr<Move<ESolutionTSP>> indexMove(unsigned int k) override
   {
      int i = k / (n - 1);
      int j = k % (n - 1) + 1;

      // last special detail...
      if (i >= j) {
         i = (n - 1) - i;
         j = (n - 1) - j + 1;
      }

      return uptr<Move<ESolutionTSP>>(new MoveSwap(i, j, *pI));

      // Please, keep 'busca' for historical (and emotional) purposes :)
      // This was created in the night before the TCC presentation of OptFrame (in 2009)
      // And now, in 2017, a beautiful calculation is presented.
      //return busca(k, 1, 2 * n);
   }

   unsigned int size() const
   {
      return n * (n - 1) / 2;
   }

   virtual void print() const
   {
      cout << "NSEnum Swap (" << size() << ")\n";
   }

   // Auxiliar methods

   int corresp(int d)
   {
      return d - ((d - (n - 1)) - 1) * 2;
   }

   int numElem(int d)
   {
      if (d <= n)
         return (d / 2);
      else
         return numElem(corresp(d));
   }

   int comeca(int d)
   {

      if (d <= n) {
         int z = (d / 2);

         int ant = z * (z - 1);

         // Se impar, soma mais 'z'
         if (d % 2 == 1)
            ant += z;

         return ant;
      } else {
         return 2 * (comeca(n)) - comeca(corresp(d) + 1) + numElem(n);
      }
   }

   int termina(int d)
   {
      return comeca(d) + numElem(d) - 1;
   }

   Move<ESolutionTSP>& busca(int k, int a, int b)
   {
      int d = (a + b) / 2;

      //cout << "busca "<<k<<" na diagonal "<<d<<"entre ["<<a<<","<<b<<"]"<<endl;

      int c = comeca(d);
      int t = termina(d);

      //cout <<"comeca em "<<c<<" e termina em "<<t<<endl;

      //int p;
      //cin >>p;

      if (k < c) {
         //cout << "k<c"<<endl;
         return busca(k, a, d);
      }

      if (k > t) {
         //cout << "k>t"<<endl;

         if (a == d)
            d++;

         return busca(k, d, b);
      }

      if (d <= n) {

         for (int i = 0; i < numElem(d); i++)
            if (k == c + i)
               return *new MoveSwap(i, d - i - 1, *pI);
      } else {

         for (int i = 0; i < numElem(d); i++)
            if (k == c + i) {
               int j = d - n;
               return *new MoveSwap(i + j, d - i - 1 - j, *pI);
            }
      }

      cout << "Error!" << endl;
      return *new MoveSwap(0, 0, *pI);
   }
};

} // end namespace TSP

#endif /*TSP_NSENUMSwap_HPP_*/
