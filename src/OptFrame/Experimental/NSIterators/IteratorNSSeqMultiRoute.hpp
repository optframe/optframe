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

#ifndef OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"

using namespace std;

class NSSeqMultiRouteIteratorOutOfBound
{
public:
   NSSeqMultiRouteIteratorOutOfBound()
   {
   }
};

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MoveMultiRoute<T, DS>>
class IteratorNSSeqMultiRoute : public NSIterator<vector<vector<T>>, DS>
{
   typedef vector<T> Route;
   typedef vector<vector<T>> MultiRoute;

private:
   vector<NSIterator<Route, DS>*>& iterators;
   int i;

public:
   IteratorNSSeqMultiRoute(vector<NSIterator<Route, DS>*>& it)
     : iterators(it)
   {
      i = 0;
   }

   virtual ~IteratorNSSeqMultiRoute()
   {
      for (int j = 0; j < iterators.size(); j++)
         delete iterators[j];
      delete &iterators;
   }

   void first()
   {
      for (int j = 0; j < iterators.size(); j++)
         iterators[j]->first();

      i = 0;
      while (i < iterators.size())
         if (!iterators[i]->isDone())
            break;
         else
            i++;
   }

   void next()
   {
      iterators[i]->next();
      while (i < iterators.size())
         if (!iterators[i]->isDone())
            break;
         else
            i++;
   }

   bool isDone()
   {
      for (int j = i; j < iterators.size(); j++)
         if (!iterators[j]->isDone())
            return false;
      return true;
   }

   Move<MultiRoute, DS>& current()
   {
      if ((i < iterators.size()) && (!iterators[i]->isDone()))
         return *new MOVE(i, iterators[i]->current());
      else
         throw NSSeqMultiRouteIteratorOutOfBound();
   }
};

#endif /*OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_*/
