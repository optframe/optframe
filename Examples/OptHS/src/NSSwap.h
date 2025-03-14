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

#ifndef OptHS_NSSwap_HPP_
#define OptHS_NSSwap_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>

// #include "../../OptFrame/Util/TestMove.hpp"
#include <OptFrame/Core/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace OptHS {

class MoveSwap : public Move<ESolutionOptHS> {
 public:
  int w1;
  int w2;

  MoveSwap(int _w1, int _w2) : w1(_w1), w2(_w2) {}

  virtual ~MoveSwap() {}

  bool canBeApplied(const ESolutionOptHS& s) override { return w1 != w2; }

  uptr<Move<ESolutionOptHS>> apply(ESolutionOptHS& se) override {
    RepOptHS& rep = se.first.getR();
    pair<char, char> aux = rep[w1];
    rep[w1] = rep[w2];
    rep[w2] = aux;
    return uptr<Move<ESolutionOptHS>>(new MoveSwap(w2, w1));
  }

  virtual bool operator==(const Move<ESolutionOptHS>& _m) const {
    const MoveSwap& m = (const MoveSwap&)_m;
    return (w1 == m.w1) && (w2 == m.w2);
  }

  void print() const override {
    std::cout << "MoveSwap: (" << w1 << "," << w2 << ")" << std::endl;
  }
};

class NSSwap : public NS<ESolutionOptHS> {
 public:
  ProblemInstance& p;
  RandGen& rg;

  NSSwap(ProblemInstance& _p, RandGen& _rg) : p(_p), rg(_rg) {}

  virtual ~NSSwap() {}

  virtual uptr<Move<ESolutionOptHS>> randomMove(
      const ESolutionOptHS& se) override {
    const RepOptHS& rep = se.first.getR();
    int w1 = rg.rand(rep.size());
    int w2 = w1;
    while (w2 == w1) w2 = rg.rand(rep.size());

    return uptr<Move<ESolutionOptHS>>(new MoveSwap(w1, w2));
  }

  void print() const override { std::cout << "NSSwap" << std::endl; }
};

}  // namespace OptHS

#endif /*OptHS_NSSwap_HPP_*/
