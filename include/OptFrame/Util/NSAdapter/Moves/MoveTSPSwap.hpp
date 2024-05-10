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

#ifndef OPTFRAME_MOVETSPSWAP_HPP_
#define OPTFRAME_MOVETSPSWAP_HPP_

// C++
#include <string>
// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>
#include <OptFrame/Move.hpp>

using namespace std;  // NOLINT

// Working structure: vector<T>

namespace optframe {

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveTSPSwap : public Move<XES> {
  typedef vector<T> Route;

 protected:
  int p1, p2;  // position 1 and position 2, respectively

  OPTFRAME_DEFAULT_PROBLEM* problem;

 public:
  MoveTSPSwap(int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : p1(_p1), p2(_p2), problem(_problem) {}

  virtual ~MoveTSPSwap() {}

  int get_p1() const { return p1; }

  int get_p2() const { return p2; }

  bool canBeApplied(const XES& se) override {
    const Route& rep = se.first.getR();
    bool all_positive = (p1 >= 0) && (p2 >= 0);
    bool size_ok = (p1 < ((int)rep.size())) && (p2 < ((int)rep.size()));
    return all_positive && size_ok && (rep.size() >= 2);
  }

  uptr<Move<XES>> apply(XES& se) override {
    Route& rep = se.first.getR();
    T t = rep[p1];
    rep[p1] = rep[p2];
    rep[p2] = t;

    return uptr<Move<XES>>(new MoveTSPSwap(p1, p2));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveTSPSwap& m1 = (const MoveTSPSwap&)_m;
    return ((m1.p1 == p1) && (m1.p2 == p2));
  }

  void print() const override {
    cout << "MoveTSPSwap( " << p1 << " <=> " << p2 << " )" << endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOVETSPSWAP_HPP_*/
