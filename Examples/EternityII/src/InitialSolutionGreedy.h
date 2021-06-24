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

#ifndef EtII_INITIALSOLUTION_Greedy_HPP_
#define EtII_INITIALSOLUTION_Greedy_HPP_

#include <OptFrame/Constructive.hpp>
//#include "../../OptFrame/Util/TestSolution.hpp"
#include <OptFrame/RandGen.hpp>

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;
using namespace optframe;

namespace EtII {

class EtIIInitialSolutionGreedy : public Constructive<SolutionEtII>
{
private:
   ProblemInstance& pEtII;
   RandGen& rg;

public:
   EtIIInitialSolutionGreedy(ProblemInstance& _pEtII, RandGen& _rg)
     : // If necessary, add more parameters
     pEtII(_pEtII)
     , rg(_rg)
   {
   }

   std::optional<SolutionEtII> generateSolution(double timelimit) override
   {
      RepEtII* tab = new RepEtII(pEtII.height, pEtII.width);

      vector<Piece> corner_pieces;
      vector<Piece> side_pieces;
      vector<Piece> center_pieces;

      for (unsigned int i = 0; i < pEtII.pieces.size(); i++) {
         int zeros = 0;

         if (pEtII.pieces[i].down == 0)
            zeros++;
         if (pEtII.pieces[i].left == 0)
            zeros++;
         if (pEtII.pieces[i].up == 0)
            zeros++;
         if (pEtII.pieces[i].right == 0)
            zeros++;

         if (zeros == 2)
            corner_pieces.push_back(pEtII.pieces[i]);
         if (zeros == 1)
            side_pieces.push_back(pEtII.pieces[i]);
         if (zeros == 0)
            center_pieces.push_back(pEtII.pieces[i]);
      }

      // shuffle elements
      rg.shuffle(corner_pieces);
      rg.shuffle(side_pieces);
      rg.shuffle(center_pieces);

      int x;
      Piece p;

      // random one piece for left-upper corner
      x = rg.rand(corner_pieces.size());
      p = corner_pieces[x];
      corner_pieces.erase(corner_pieces.begin() + x);

      while ((p.left != 0) || (p.up != 0))
         p.rotate();

      (*tab)(0, 0) = p;

      // random one piece for right-upper corner
      x = rg.rand(corner_pieces.size());
      p = corner_pieces[x];
      corner_pieces.erase(corner_pieces.begin() + x);

      while ((p.right != 0) || (p.up != 0))
         p.rotate();

      (*tab)(0, tab->getNumCols() - 1) = p;

      // random one piece for right-lower corner
      x = rg.rand(corner_pieces.size());
      p = corner_pieces[x];
      corner_pieces.erase(corner_pieces.begin() + x);

      while ((p.right != 0) || (p.down != 0))
         p.rotate();

      (*tab)(tab->getNumRows() - 1, tab->getNumCols() - 1) = p;

      // random one piece for left-lower corner
      x = rg.rand(corner_pieces.size());
      p = corner_pieces[x];
      corner_pieces.erase(corner_pieces.begin() + x);

      while ((p.left != 0) || (p.down != 0))
         p.rotate();

      (*tab)(tab->getNumRows() - 1, 0) = p;

      // choose the pieces for upper-bottom sides
      for (unsigned int i = 1; i < tab->getNumCols() - 1; i++) {
         // top
         //int max = 0;
         int best = 0;
         for (unsigned int j = 0; j < side_pieces.size(); j++) {
            p = side_pieces[j];

            while (p.up != 0)
               p.rotate();

            int f = 0;
            if (p.left == (*tab)(0, i - 1).right) {
               f++;
               // best found!
               //max = f;
               best = j;
               break;
            }
         }

         p = side_pieces[best];
         side_pieces.erase(side_pieces.begin() + best);

         while (p.up != 0)
            p.rotate();
         (*tab)(0, i) = p;

         // bottom
         //max = 0;
         best = 0;
         for (unsigned int j = 0; j < side_pieces.size(); j++) {
            p = side_pieces[j];

            while (p.down != 0)
               p.rotate();

            int f = 0;
            if (p.left == (*tab)(tab->getNumRows() - 1, i - 1).right) {
               f++;
               // best found!
               //max = f;
               best = j;
               break;
            }
         }

         p = side_pieces[best];
         side_pieces.erase(side_pieces.begin() + best);

         while (p.down != 0)
            p.rotate();
         (*tab)(tab->getNumRows() - 1, i) = p;
      }

      // random the pieces for left-right sides
      for (unsigned int i = 1; i < tab->getNumRows() - 1; i++) {
         // left
         //int max = 0;
         int best = 0;
         for (unsigned int j = 0; j < side_pieces.size(); j++) {
            p = side_pieces[j];

            while (p.left != 0)
               p.rotate();

            int f = 0;
            if (p.up == (*tab)(i - 1, 0).down) {
               f++;
               // best found!
               //max = f;
               best = j;
               break;
            }
         }

         p = side_pieces[best];
         side_pieces.erase(side_pieces.begin() + best);

         while (p.left != 0)
            p.rotate();
         (*tab)(i, 0) = p;

         // right
         //max = 0;
         best = 0;
         for (unsigned int j = 0; j < side_pieces.size(); j++) {
            p = side_pieces[j];

            while (p.right != 0)
               p.rotate();

            int f = 0;
            if (p.up == (*tab)(i - 1, tab->getNumCols() - 1).down) {
               f++;
               // best found!
               //max = f;
               best = j;
               break;
            }
         }

         p = side_pieces[best];
         side_pieces.erase(side_pieces.begin() + best);

         while (p.right != 0)
            p.rotate();
         (*tab)(i, tab->getNumCols() - 1) = p;
      }

      // choose the center pieces
      for (unsigned int i = 1; i < tab->getNumRows() - 1; i++)
         for (unsigned int j = 1; j < tab->getNumCols() - 1; j++) {
            int max = -1;
            int best_k = -1;
            int best_r = -1;

            for (unsigned int k = 0; k < center_pieces.size(); k++) {
               p = center_pieces[k];

               for (int r = 0; r < 4; r++) {
                  int f = 0;
                  if (p.left == (*tab)(i, j - 1).right)
                     f++;
                  if (p.up == (*tab)(i - 1, j).down)
                     f++;

                  if (f > max) {
                     max = f;
                     best_k = k;
                     best_r = r;
                  }

                  p.rotate();
               }

               // last rotation
               p.rotate();

               if (max == 2) // best piece
                  break;
            }

            p = center_pieces[best_k];
            center_pieces.erase(center_pieces.begin() + best_k);

            for (int r = 0; r < best_r; r++)
               p.rotate();

            (*tab)(i, j) = p;
         }

      if (corner_pieces.size() > 0 || side_pieces.size() > 0 || center_pieces.size() > 0) {
         cout << "Warning: construction problem!" << endl;
      }

      return make_optional(SolutionEtII(*tab));
   }
};
}

#endif /*EtII_INITIALSOLUTION_Greedy_HPP_*/
