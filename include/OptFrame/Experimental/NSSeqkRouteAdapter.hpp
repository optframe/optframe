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

#ifndef OPTFRAME_NSSEQkROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQkROUTEADAPTER_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>

#include "./Moves/MovekRoute.hpp"
#include "./NSIterators/IteratorNSSeqkRoute.hpp"

using namespace std;

template <class T, class DS = OPTFRAME_DEFAULT_EMEMORY,
          class MOVE = MovekRoute<T, DS>>
class NSSeqkRouteAdapter : public NSSeq<vector<vector<T>>, DS> {
  typedef vector<T> Route;
  typedef vector<vector<T>> MultiRoute;

 private:
  int k;
  NSSeq<Route, DS>& ns;

 public:
  NSSeqkRouteAdapter(int _k, NSSeq<Route, DS>& _ns) : k(_k), ns(_ns) {}

  virtual ~NSSeqkRouteAdapter() {}

  Move<MultiRoute, DS>& move(const MultiRoute& r) {
    return *new MOVE(k, ns.move(r[k]));
  }

  virtual NSIterator<MultiRoute, DS>& getIterator(const MultiRoute& r) {
    NSIterator<Route, DS>& iterator = ns.getIterator(r[k]);
    return *new IteratorNSSeqkRoute<T, DS, MOVE>(k, iterator);
  }

  virtual void print() {
    std::cout << "NSSeqkRouteAdapter {" << std::endl;
    ns.print();
    std::cout << "}" << std::endl;
  }
};

#endif /*OPTFRAME_NSSEQkROUTEADAPTER_HPP_*/
