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

#ifndef OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_

#include <vector>

// Framework includes

#include <OptFrame/Core/NSSeq.hpp>

#include "../Move.hpp"
#include "../NSIterator.hpp"
#include "Moves/MoveMultiRoute.hpp"
#include "NSIterators/IteratorNSSeqMultiRoute.hpp"

using namespace std;

template <class T, class DS = OPTFRAME_DEFAULT_EMEMORY,
          class MOVE = MoveMultiRoute<T, DS>>
class NSSeqMultiRouteAdapter : public NSSeq<vector<vector<T>>, DS> {
  typedef vector<T> Route;
  typedef vector<vector<T>> MultiRoute;

 private:
  NSSeq<Route, DS>& ns;

 public:
  NSSeqMultiRouteAdapter(NSSeq<Route, DS>& _ns) : ns(_ns) {}

  virtual ~NSSeqMultiRouteAdapter() {}

  Move<MultiRoute, DS>& move(const MultiRoute& r) {
    int x = rand() % r.size();
    return *new MOVE(x, ns.move(r[x]));
  }

  virtual NSIterator<MultiRoute, DS>& getIterator(const MultiRoute& r) {
    vector<NSIterator<Route, DS>*>& iterators =
        *new vector<NSIterator<Route, DS>*>;
    for (int i = 0; i < r.size(); i++)
      iterators.push_back(&ns.getIterator(r[i]));

    return *new IteratorNSSeqMultiRoute<T, DS, MOVE>(iterators);
  }

  virtual void print() {
    std::cout << "NSSeqMultiRouteAdapter {" << std::endl;
    ns.print();
    std::cout << "}" << std::endl;
  }
};

#endif /*OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_*/
