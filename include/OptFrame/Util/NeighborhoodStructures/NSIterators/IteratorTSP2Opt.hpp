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

#ifndef OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_

// Framework includes
#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"
#include "../Moves/MoveTSP2Opt.hpp"

using namespace std;
using namespace optframe;

// Working structure: vector<T>

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS,
// XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE =
// MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv =
// Evaluation<>>
template <class T, class ADS, XBaseSolution<vector<T>, ADS> S,
          class MOVE = MoveTSP2Opt<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class NSIteratorTSP2Opt : public NSIterator<XES> {
  typedef vector<T> Route;

 protected:
  uptr<Move<XES>> m;

  int p1, p2;  // position 1 and position 2, respectively
  const Route& r;

  std::shared_ptr<P> p;  // has to be the last

 public:
  NSIteratorTSP2Opt(const S& _s, std::shared_ptr<P> _p = nullptr)
      : r(_s.getR()), p(_p) {
    p1 = 0;  // avoid non-initialization warning
    p2 = 2;  // avoid non-initialization warning

    m = nullptr;
  }

  virtual ~NSIteratorTSP2Opt() {}

  virtual void first() override {
    if (r.size() >= 2) {
      p1 = 0;
      p2 = 2;
      m = uptr<MOVE>(new MOVE(p1, p2, p));
    } else
      m = nullptr;
  }

  virtual void next() override {
    if (!((p1 == ((int)r.size()) - 2) && (p2 == ((int)r.size())))) {
      if (p2 != ((int)r.size())) {
        p2++;
      } else {
        p1++;
        p2 = p1 + 2;
      }

      MOVE* pm = new MOVE(p1, p2, p);
      m = uptr<Move<XES>>(pm);
    } else
      m = nullptr;
  }

  virtual bool isDone() override { return m == nullptr; }

  virtual uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqRouteShift. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

#endif /*OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_*/
