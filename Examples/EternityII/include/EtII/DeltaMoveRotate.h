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

#ifndef EtII_Delta_Move_Rotate_HPP_
#define EtII_Delta_Move_Rotate_HPP_

#include <cmath>

#include "./NSSeqRotate.h"

using namespace std;

namespace EtII {

class DeltaMoveRotate : public MoveRotate {
 public:
  DeltaMoveRotate(int _nRot, int _x, int _y) : MoveRotate(_nRot, _x, _y) {}

  virtual ~DeltaMoveRotate() {}

  op<Evaluation<>> cost(const pair<SolutionEtII, Evaluation<>>& se,
                        bool allowEstimated) override {
    const SolutionEtII& s = se.first;
    // const Evaluation<>& e = se.second;

    const RepEtII& rep = s.getR();
    double f = 0;

    int f1 = 0;
    if (rep(x, y).left == rep(x, y - 1).right) f1++;
    if (rep(x, y).up == rep(x - 1, y).down) f1++;
    if (rep(x, y).right == rep(x, y + 1).left) f1++;
    if (rep(x, y).down == rep(x + 1, y).up) f1++;

    int f2 = 0;
    if (nRot == 1) {
      if (rep(x, y).left == rep(x - 1, y).down) f2++;
      if (rep(x, y).up == rep(x, y + 1).left) f2++;
      if (rep(x, y).right == rep(x + 1, y).up) f2++;
      if (rep(x, y).down == rep(x, y - 1).right) f2++;
    }

    if (nRot == 2) {
      if (rep(x, y).left == rep(x, y + 1).left) f2++;
      if (rep(x, y).up == rep(x + 1, y).up) f2++;
      if (rep(x, y).right == rep(x, y - 1).right) f2++;
      if (rep(x, y).down == rep(x - 1, y).down) f2++;
    }

    if (nRot == 3) {
      if (rep(x, y).left == rep(x + 1, y).up) f2++;
      if (rep(x, y).up == rep(x, y - 1).right) f2++;
      if (rep(x, y).right == rep(x - 1, y).down) f2++;
      if (rep(x, y).down == rep(x, y + 1).left) f2++;
    }

    f += (f2 - f1);

    // return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static std::string idComponent() {
    string idComp = MoveRotate::idComponent();
    idComp.append(":DeltaMoveRotate");
    return idComp;
  }

  virtual bool operator==(const DeltaMoveRotate& _m) const {
    const DeltaMoveRotate& m = (const DeltaMoveRotate&)_m;
    return (m.nRot == nRot) && (m.x == x) && (m.y == y);
  }

  void print() const override {
    std::cout << "DeltaMoveRotate( ";
    std::cout << nRot << " rotations on (" << x << "," << y << ")" << std::endl;
    std::cout << std::endl;
  }

  std::string id() const override { return "OptFrame:Move:DeltaMoveRotate"; }
};
}  // namespace EtII
#endif /*EtII_Delta_Move_Rotate_HPP_*/
