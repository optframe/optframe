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

#ifndef OPTFRAME_MOVEMULTIROUTE_HPP_
#define OPTFRAME_MOVEMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template <class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MoveMultiRoute : public Move<vector<vector<T>>, DS> {
  typedef vector<T> Route;
  typedef vector<vector<T>> MultiRoute;

 protected:
  int k;  // route number
  Move<Route, DS>& m;

 public:
  MoveMultiRoute(int _k, Move<Route, DS>& _m) : k(_k), m(_m) {}

  int get_k() { return k; }

  Move<Route, DS>& get_m() { return m; }

  virtual ~MoveMultiRoute() { delete &m; }

  bool canBeApplied(const MultiRoute& rep) { return m.canBeApplied(rep[k]); }

  Move<MultiRoute, DS>& apply(MultiRoute& rep) {
    return *new MoveMultiRoute<T, DS>(k, m.apply(rep[k]));
  }

  Move<MultiRoute, DS>& apply(DS& mem, MultiRoute& rep) {
    return *new MoveMultiRoute<T, DS>(k, m.apply(mem, rep[k]));
  }

  virtual bool operator==(const Move<MultiRoute, DS>& _m) const {
    const MoveMultiRoute<T, DS>& m1 = (const MoveMultiRoute<T, DS>&)_m;
    if (k == m1.k)
      return m == m1.m;
    else
      return false;
  }

  void print() const override {
    cout << "MoveMultiRoute: k=" << k << "; move = ";
    m.print();
  }
};

#endif /*OPTFRAME_MOVEMULTIROUTE_HPP_*/
