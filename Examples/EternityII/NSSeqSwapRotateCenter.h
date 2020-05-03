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

#ifndef EtII_NSSEQSwapRotateCenter_HPP_
#define EtII_NSSEQSwapRotateCenter_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace EtII {

class MoveSwapRotateCenter : public Move<ESolutionEtII>
{
protected:
   int x1, y1, x2, y2;
   int r1, r2;

public:
   using Move<ESolutionEtII>::apply;        // prevents name hiding
   using Move<ESolutionEtII>::canBeApplied; // prevents name hiding

   MoveSwapRotateCenter(int _x1, int _y1, int _x2, int _y2)
     : x1(_x1)
     , y1(_y1)
     , x2(_x2)
     , y2(_y2)
   {
      r1 = -1;
      r2 = -1;
   }

private:
   MoveSwapRotateCenter(int _x1, int _y1, int _r1, int _x2, int _y2, int _r2)
     : x1(_x1)
     , y1(_y1)
     , x2(_x2)
     , y2(_y2)
   {
      r1 = _r1;
      r2 = _r2;
   }

public:
   virtual ~MoveSwapRotateCenter()
   {
   }

   bool canBeApplied(const ESolutionEtII& s) override
   {
      return !((x1 == x2) && (y1 == y2));
   }

   uptr<Move<ESolutionEtII>> apply(ESolutionEtII& se) override
   {
      RepEtII& rep = se.first.getR();
      Piece p = rep(x1, y1);
      rep(x1, y1) = rep(x2, y2);
      rep(x2, y2) = p;

      int rot1, rot2;

      if (r1 < 0) {
         int best_r = -1;
         int best_f = -1;

         for (int i = 0; i < 4; i++) {
            int f = 0;
            if (rep(x1, y1).left == rep(x1, y1 - 1).right)
               f++;
            if (rep(x1, y1).up == rep(x1 - 1, y1).down)
               f++;
            if (rep(x1, y1).right == rep(x1, y1 + 1).left)
               f++;
            if (rep(x1, y1).down == rep(x1 + 1, y1).up)
               f++;

            if (f > best_f) {
               best_f = f;
               best_r = i;
            }

            rep(x1, y1).rotate();
         }

         for (int r = 0; r < best_r; r++)
            rep(x1, y1).rotate();

         rot1 = 4 - best_r;
         if (rot1 == 4)
            rot1 = 0;

         // =============

         best_r = -1;
         best_f = -1;

         for (int i = 0; i < 4; i++) {
            int f = 0;
            if (rep(x2, y2).left == rep(x2, y2 - 1).right)
               f++;
            if (rep(x2, y2).up == rep(x2 - 1, y2).down)
               f++;
            if (rep(x2, y2).right == rep(x2, y2 + 1).left)
               f++;
            if (rep(x2, y2).down == rep(x2 + 1, y2).up)
               f++;

            if (f > best_f) {
               best_f = f;
               best_r = i;
            }

            rep(x2, y2).rotate();
         }

         for (int r = 0; r < best_r; r++)
            rep(x2, y2).rotate();

         rot2 = 4 - best_r;
         if (rot2 == 4)
            rot2 = 0;
      } else {
         for (int i = 0; i < r1; i++)
            rep(x1, y1).rotate();

         for (int i = 0; i < r2; i++)
            rep(x2, y2).rotate();

         rot1 = -1;
         rot2 = -1;
      }

      return uptr<Move<ESolutionEtII>>(new MoveSwapRotateCenter(x1, y1, rot2, x2, y2, rot1));
   }

   uptr<Move<ESolutionEtII>> applyUpdate(pair<SolutionEtII, Evaluation<>>& se) override
   {
      SolutionEtII& s = se.first;
      Evaluation<>& e = se.second;

      RepEtII& rep = s.getR();
      int f = 0;
      if (rep(x1, y1).left == rep(x1, y1 - 1).right)
         f++;
      if (rep(x1, y1).up == rep(x1 - 1, y1).down)
         f++;
      if (rep(x1, y1).right == rep(x1, y1 + 1).left)
         f++;
      if (rep(x1, y1).down == rep(x1 + 1, y1).up)
         f++;

      int g = 0;
      if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
         g++;
      if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
         g++;
      if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
         g++;
      if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
         g++;

      uptr<Move<ESolutionEtII>> rev = apply(se);

      int f2 = 0;
      if (rep(x1, y1).left == rep(x1, y1 - 1).right)
         f2++;
      if (rep(x1, y1).up == rep(x1 - 1, y1).down)
         f2++;
      if (rep(x1, y1).right == rep(x1, y1 + 1).left)
         f2++;
      if (rep(x1, y1).down == rep(x1 + 1, y1).up)
         f2++;

      int g2 = 0;
      if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
         g2++;
      if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
         g2++;
      if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
         g2++;
      if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
         g2++;

      e.setObjFunction(e.getObjFunction() + (f2 - f));
      e.setObjFunction(e.getObjFunction() + (g2 - g));

      // keeping 'outdated' status untouched... should avoid re-evaluations!

      return rev;
   }

   virtual bool operator==(const Move<ESolutionEtII>& _m) const
   {
      const MoveSwapRotateCenter& m = (const MoveSwapRotateCenter&)_m;
      return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2) && (m.r1 == r1) && (m.r2 == r2);
   }

   void print() const
   {
      cout << "MoveSwapRotateCenter: (" << x1 << "," << y1 << ") r=" << r1 << " <=> (" << x2 << "," << y2 << ") r=" << r2 << endl;
   }

   string id() const
   {
      return "OptFrame:Move:MoveSwapRotateCenter";
   }
};

class NSIteratorSwapRotateCenter : public NSIterator<ESolutionEtII>
{
private:
   int nIntraRows, nIntraCols;
   int x1, y1, x2, y2;

public:
   NSIteratorSwapRotateCenter(int _nIntraRows, int _nIntraCols)
     : nIntraRows(_nIntraRows)
     , nIntraCols(_nIntraCols)
   {
   }

   virtual ~NSIteratorSwapRotateCenter()
   {
   }

   virtual void first() override
   {
      x1 = 0;
      y1 = 0;

      x2 = 0;
      y2 = 1;
   }

   virtual void next() override
   {
      y2++;
      if (y2 >= nIntraCols) {
         y2 = 0;
         x2++;
         if (x2 >= nIntraRows) {
            y1++;
            if (y1 >= nIntraCols) {
               y1 = 0;
               x1++;

               x2 = x1;
               y2 = y1 + 1;
               if (y2 >= nIntraCols) {
                  y2 = 0;
                  x2++;
               }
            } else {
               x2 = x1;
               y2 = y1 + 1;
               if (y2 >= nIntraCols) {
                  y2 = 0;
                  x2++;
               }
            }
         }
      }
   }

   virtual bool isDone() override
   {
      return (x1 >= nIntraRows) || (y1 >= nIntraCols) || (x2 >= nIntraRows) || (y2 >= nIntraCols);
   }

   virtual uptr<Move<ESolutionEtII>> current() override
   {
      return uptr<Move<ESolutionEtII>>(new MoveSwapRotateCenter(x1 + 1, y1 + 1, x2 + 1, y2 + 1));
   }
};

template<class MOVE = MoveSwapRotateCenter>
class NSSeqSwapRotateCenter : public NSSeq<ESolutionEtII>
{
private:
   RandGen& rg;

public:
   NSSeqSwapRotateCenter(RandGen& _rg)
     : rg(_rg)
   {
   }

   virtual ~NSSeqSwapRotateCenter()
   {
   }

   virtual uptr<Move<ESolutionEtII>> randomMove(const ESolutionEtII& se) override
   {
      const RepEtII& rep = se.first.getR();
      if ((rep.getNumRows() == 3) && (rep.getNumCols() == 3))
         return uptr<Move<ESolutionEtII>>(new MoveSwapRotateCenter(1, 1, 1, 1));

      // line 'x' and col 'y'
      int x1 = rg.rand((rep.getNumRows() - 2)) + 1;
      int y1 = rg.rand((rep.getNumCols() - 2)) + 1;

      int x2 = x1;
      int y2 = y1;
      while ((x2 == x1) && (y2 == y1)) {
         x2 = rg.rand((rep.getNumRows() - 2)) + 1;
         y2 = rg.rand((rep.getNumCols() - 2)) + 1;
      }

      return uptr<Move<ESolutionEtII>>(new MOVE(x1, y1, x2, y2));
   }

   virtual uptr<NSIterator<ESolutionEtII>> getIterator(const ESolutionEtII& se) override
   {
      const RepEtII& rep = se.first.getR();
      // return an iterator to the neighbors of 'rep'
      return uptr<NSIterator<ESolutionEtII>>(new NSIteratorSwapRotateCenter(rep.getNumRows() - 2, rep.getNumCols() - 2));
   }

   virtual void print() const
   {
      cout << "NSSeqSwapRotateCenter" << endl;
   }

   string id() const
   {
      return "OptFrame:NS:NSSeqSwapRotateCenter";
   }
};
}

#endif /*EtII_NSSEQSwapRotateCenter_HPP_*/
