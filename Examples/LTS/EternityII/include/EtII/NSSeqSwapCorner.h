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

#ifndef EtII_NSSEQSwapCorner_HPP_
#define EtII_NSSEQSwapCorner_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Core/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace EtII {

class MoveSwapCorner : public Move<ESolutionEtII> {
 protected:
  int x1, y1, x2, y2;

 public:
  using Move<ESolutionEtII>::apply;         // prevents name hiding
  using Move<ESolutionEtII>::canBeApplied;  // prevents name hiding

  MoveSwapCorner(int _x1, int _y1, int _x2, int _y2)
      : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

  virtual ~MoveSwapCorner() {}

  bool canBeApplied(const ESolutionEtII& s) override { return true; }

  uptr<Move<ESolutionEtII>> apply(ESolutionEtII& se) override {
    RepEtII& rep = se.first.getR();
    Piece p = rep(x1, y1);
    rep(x1, y1) = rep(x2, y2);
    rep(x2, y2) = p;

    while ((rep(0, 0).left != 0) || (rep(0, 0).up != 0)) rep(0, 0).rotate();

    while ((rep(0, rep.getNumCols() - 1).right != 0) ||
           (rep(0, rep.getNumCols() - 1).up != 0))
      rep(0, rep.getNumCols() - 1).rotate();

    while ((rep(rep.getNumRows() - 1, 0).left != 0) ||
           (rep(rep.getNumRows() - 1, 0).down != 0))
      rep(rep.getNumRows() - 1, 0).rotate();

    while ((rep(rep.getNumRows() - 1, rep.getNumCols() - 1).right != 0) ||
           (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).down != 0))
      rep(rep.getNumRows() - 1, rep.getNumCols() - 1).rotate();

    return uptr<Move<ESolutionEtII>>(new MoveSwapCorner(x2, y2, x1, y1));
  }

  uptr<Move<ESolutionEtII>> applyUpdate(
      pair<SolutionEtII, Evaluation<>>& se) override {
    SolutionEtII& s = se.first;
    Evaluation<>& e = se.second;

    RepEtII& rep = s.getR();
    int f = 0;

    if (rep(0, 0).right == rep(0, 1).left) f++;
    if (rep(0, 0).down == rep(1, 0).up) f++;

    if (rep(0, rep.getNumCols() - 1).left == rep(0, rep.getNumCols() - 2).right)
      f++;
    if (rep(0, rep.getNumCols() - 1).down == rep(1, rep.getNumCols() - 1).up)
      f++;

    if (rep(rep.getNumRows() - 1, 0).right == rep(rep.getNumRows() - 1, 1).left)
      f++;
    if (rep(rep.getNumRows() - 1, 0).up == rep(rep.getNumRows() - 2, 0).down)
      f++;

    if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).left ==
        rep(rep.getNumRows() - 1, rep.getNumCols() - 2).right)
      f++;
    if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).up ==
        rep(rep.getNumRows() - 2, rep.getNumCols() - 1).down)
      f++;

    uptr<Move<ESolutionEtII>> rev = apply(se);

    int f2 = 0;

    if (rep(0, 0).right == rep(0, 1).left) f2++;
    if (rep(0, 0).down == rep(1, 0).up) f2++;

    if (rep(0, rep.getNumCols() - 1).left == rep(0, rep.getNumCols() - 2).right)
      f2++;
    if (rep(0, rep.getNumCols() - 1).down == rep(1, rep.getNumCols() - 1).up)
      f2++;

    if (rep(rep.getNumRows() - 1, 0).right == rep(rep.getNumRows() - 1, 1).left)
      f2++;
    if (rep(rep.getNumRows() - 1, 0).up == rep(rep.getNumRows() - 2, 0).down)
      f2++;

    if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).left ==
        rep(rep.getNumRows() - 1, rep.getNumCols() - 2).right)
      f2++;
    if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).up ==
        rep(rep.getNumRows() - 2, rep.getNumCols() - 1).down)
      f2++;

    e.setObjFunction(e.getObjFunction() + (f2 - f));

    return rev;
  }

  bool operator==(const optframe::Move<ESolutionEtII>& _m) const override {
    const MoveSwapCorner& m = (const MoveSwapCorner&)_m;
    return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
  }

  void print() const override {
    std::cout << "MoveSwapCorner: (" << x1 << "," << y1 << ") <=> (" << x2
              << "," << y2 << ")" << std::endl;
  }

  std::string id() const override { return "OptFrame:Move:MoveSwapCorner"; }
};

class NSIteratorSwapCorner : public NSIterator<ESolutionEtII> {
 private:
  int x1, y1, x2, y2;
  int nRows, nCols;

 public:
  NSIteratorSwapCorner(int _nRows, int _nCols) : nRows(_nRows), nCols(_nCols) {}

  virtual ~NSIteratorSwapCorner() {}

  virtual void first() override {
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 1;
  }

  virtual void next() override {
    y2++;
    if (y2 > 1) {
      y2 = 0;
      x2++;
      if (x2 > 1) {
        y1++;
        if (y1 > 1) {
          y1 = 0;
          x1++;

          x2 = x1;
          y2 = y1 + 1;
          if (y2 > 1) {
            y2 = 0;
            x2++;
          }
        } else {
          x2 = x1;
          y2 = y1 + 1;
          if (y2 > 1) {
            y2 = 0;
            x2++;
          }
        }
      }
    }
  }

  virtual bool isDone() override { return x2 > 1; }

  virtual uptr<Move<ESolutionEtII>> current() override {
    return uptr<Move<ESolutionEtII>>(
        new MoveSwapCorner(x1 * (nRows - 1), y1 * (nCols - 1), x2 * (nRows - 1),
                           y2 * (nCols - 1)));
  }
};

template <class MOVE = MoveSwapCorner>
class NSSeqSwapCorner : public NSSeq<ESolutionEtII> {
 private:
  RandGen& rg;

 public:
  NSSeqSwapCorner(RandGen& _rg) : rg(_rg) {}

  virtual ~NSSeqSwapCorner() {}

  virtual uptr<Move<ESolutionEtII>> randomMove(
      const ESolutionEtII& se) override {
    const RepEtII& rep = se.first.getR();
    int x1 = (rg.rand(2)) * (rep.getNumRows() - 1);
    int y1 = (rg.rand(2)) * (rep.getNumCols() - 1);

    int x2 = x1;
    int y2 = y1;

    while ((x2 == x1) && (y2 == y1)) {
      x2 = (rg.rand(2)) * (rep.getNumRows() - 1);
      y2 = (rg.rand(2)) * (rep.getNumCols() - 1);
    }

    return uptr<Move<ESolutionEtII>>(new MOVE(x1, y1, x2, y2));
  }

  virtual uptr<NSIterator<ESolutionEtII>> getIterator(
      const ESolutionEtII& se) override {
    const RepEtII& rep = se.first.getR();
    // return an iterator to the neighbors of 'rep'
    return uptr<NSIterator<ESolutionEtII>>(
        new NSIteratorSwapCorner(rep.getNumRows(), rep.getNumCols()));
  }

  void print() const override { std::cout << "NSSeqSwapCorner" << std::endl; }

  std::string id() const override {
    return "OptFrame:NS:NSFind:NSSeq:NSSeqSwapCorner";
  }
};
}  // namespace EtII

#endif /*EtII_NSSEQSwapCorner_HPP_*/
