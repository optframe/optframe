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

#ifndef OPTFRAME_MOVEVVSWAPINTRA_HPP_
#define OPTFRAME_MOVEVVSWAPINTRA_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MoveVVSwapIntra : public Move<vector<vector<T>>, DS>
{
   typedef vector<vector<T>> Routes;

private:
   int i, j, k;

public:
   MoveVVSwapIntra(int i, int j, int k)
   {
      this->i = i;
      this->j = j;
      this->k = k;
   }

   virtual ~MoveVVSwapIntra()
   {
   }

   bool canBeApplied(const Routes& rep)
   {
      return (j != k);
   }

   Move<Routes, DS>& apply(Routes& rep)
   {
      T aux = rep[i][j];
      rep[i][j] = rep[i][k];
      rep[i][k] = aux;

      return *new MoveVVSwapIntra(i, k, j);
   }

   virtual bool operator==(const Move<Routes, DS>& _m) const
   {
      const MoveVVSwapIntra& m = (const MoveVVSwapIntra&)_m;
      return (m.i == i) && ((m.j == j && m.k == k) || (m.j == k && m.k == j));
   }

   void print() const
   {
      cout << "MoveVVSwapIntra( " << i << " , ( " << j << " , " << k << " ) )" << endl;
   }
};

#endif /*OPTFRAME_MOVEVVSWAPINTRA_HPP_*/
