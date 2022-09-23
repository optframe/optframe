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

#ifndef OPTFRAME_ITERATORVVSWAPINTRA_HPP_
#define OPTFRAME_ITERATORVVSWAPINTRA_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"

#include "../Moves/MoveVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSIteratorVVSwapIntra : public NSIterator<vector<vector<T>>, DS>
{
   typedef vector<vector<T>> Routes;
   const Routes& routes;

   int m_i, m_j, m_k;

public:
   NSIteratorVVSwapIntra(const Routes& r)
     : routes(r)
   {
   }

   virtual ~NSIteratorVVSwapIntra()
   {
   }

   void first()
   {
      // initialize vars
      m_i = 0;
      m_j = 0;
      m_k = 0;

      // go to a valid move
      next();
   }

   void next()
   {
      int n2 = routes.at(m_i).size();

      if (m_k < n2 - 1)
         m_k++;
      else if (++m_j < n2 - 1) {
         m_k = m_j + 1;
      } else {
         m_i++;
         m_j = 0;
         m_k = 0;
      }
   }

   bool isDone()
   {
      int n1 = routes.size();
      int n2 = routes.at(m_i).size();

      if ((m_i >= n1 - 1) && (m_j >= n2 - 2) && (m_k >= n2 - 1))
         return true;
      else
         return false;
   }

   Move<Routes, DS>& current()
   {
      return *new MoveVVSwapIntra<T, DS>(m_i, m_j, m_k);
   }
};

#endif /*OPTFRAME_ITERATORVVSWAPINTRA_HPP_*/
