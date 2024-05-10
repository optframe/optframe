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

#ifndef OPTFRAME_ITERATORTSPSWAP_HPP_
#define OPTFRAME_ITERATORTSPSWAP_HPP_

// C++
#include <string>
// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"
#include "../Moves/MoveTSPSwap.hpp"

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

// Working structure: vector<T>

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorTSPSwap : public NSIterator<XES> {
  typedef vector<T> Route;

 protected:
  // MOVE* m;
  uptr<Move<XES>> m;
  int p1, p2;  // position 1 and position 2, respectively
  int n;

  P* p;  // has to be the last

 public:
  NSIteratorTSPSwap(int _n, P* _p = nullptr) : p(_p) {
    p1 = p2 = 0;
    n = _n;
    m = nullptr;
  }

  virtual ~NSIteratorTSPSwap() {}

  virtual void first() override {
    if (n >= 2) {
      p1 = 0;
      p2 = 1;
      m = uptr<MOVE>(new MOVE(p1, p2, p));
    } else
      m = nullptr;
  }

  virtual void next() override {
    if (!((p1 == n - 2) && (p2 == n - 1))) {
      if (p2 < (n - 1))
        p2++;

      else {
        p1++;
        p2 = p1 + 1;
      }

      m = uptr<MOVE>(new MOVE(p1, p2, p));
    } else
      m = nullptr;
  }

  virtual bool isDone() override { return (m == nullptr); }

  virtual uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqTSPSwap. Aborting." << endl;
      exit(1);
    }

    // steal from 'm'
    uptr<Move<XES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

#endif /*OPTFRAME_ITERATORTSPSWAP_HPP_*/
