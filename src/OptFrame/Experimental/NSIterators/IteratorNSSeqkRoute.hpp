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

#ifndef OPTFRAME_ITERATORNSSEQKROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQKROUTE_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"

#include "../Moves/MovekRoute.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MovekRoute<T, DS>>
class IteratorNSSeqkRoute : public NSIterator<vector<vector<T>>, DS>
{
   typedef vector<T> Route;
   typedef vector<vector<T>> MultiRoute;

private:
   int k;
   NSIterator<Route, DS>& iterator;

public:
   IteratorNSSeqkRoute(int _k, NSIterator<Route, DS>& it)
     : k(_k)
     , iterator(it)
   {
   }

   virtual ~IteratorNSSeqkRoute()
   {
      delete &iterator;
   }

   void first()
   {
      iterator.first();
   }

   void next()
   {
      iterator.next();
   }

   bool isDone()
   {
      return iterator.isDone();
   }

   Move<MultiRoute, DS>& current()
   {
      return *new MOVE(k, iterator.current());
   }
};

#endif /*OPTFRAME_ITERATORNSSEQKROUTE_HPP_*/
