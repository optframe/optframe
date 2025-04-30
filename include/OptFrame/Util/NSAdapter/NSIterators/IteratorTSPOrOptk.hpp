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

#ifndef OPTFRAME_NSITERATOROROPTK_HPP_
#define OPTFRAME_NSITERATOROROPTK_HPP_

// Framework includes
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSIterator.hpp>

#include "../Moves/MoveTSPSwap.hpp"
#include "IteratorTSPSwap.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorTSPOrOptk : public NSIterator<XES> {
  typedef vector<T> Route;

 protected:
  int n, k;
  int i, j;

  std::shared_ptr<P> p;  // has to be the last

 public:
  NSIteratorTSPOrOptk(int _n, int _k, std::shared_ptr<P> _p = nullptr)
      : n(_n), k(_k), p(_p) {
    i = j = 0;
  }

  virtual ~NSIteratorTSPOrOptk() {}

  virtual void first() override {
    i = 0;
    j = 1;
  }

  virtual void next() override {
    j++;

    if (j == i) j++;

    if (j > n - k) {
      j = 0;
      i++;
    }
  }

  bool isDone() override { return i > n - k; }

  uptr<Move<XES>> current() override {
    if (isDone()) {
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "OrOpt{K=" << k << "}. Aborting." << std::endl;
      exit(1);
    }

    return uptr<Move<XES>>(new MOVE(i, j, k, p));
  }
};

#endif /*OPTFRAME_NSITERATOROROPTK_HPP_*/
