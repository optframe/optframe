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

#ifndef EtII_Delta_Move_SwapCenter_HPP_
#define EtII_Delta_Move_SwapCenter_HPP_

#include <cmath>

#include "./NSSeqSwapCenter.h"

using namespace std;

namespace EtII {

class DeltaMoveSwapCenter : public MoveSwapCenter {
 public:
  DeltaMoveSwapCenter(int _x1, int _y1, int _x2, int _y2)
      : MoveSwapCenter(_x1, _y1, _x2, _y2) {}

  virtual ~DeltaMoveSwapCenter() {}

  class Z {
   public:
  };

  op<Evaluation<>> cost(const pair<SolutionEtII, Evaluation<>>& se,
                        bool allowEstimated) override {
    const SolutionEtII& s = se.first;
    // const Evaluation<>& e = se.second;

    const RepEtII& rep = s.getR();
    // Calculus for the slot (x1, y1)
    double f1 = 0;
    if (rep(x1, y1).left == rep(x1, y1 - 1).right) f1++;
    if (rep(x1, y1).up == rep(x1 - 1, y1).down) f1++;
    if (rep(x1, y1).right == rep(x1, y1 + 1).left) f1++;
    if (rep(x1, y1).down == rep(x1 + 1, y1).up) f1++;

    int f2 = 0;
    int neighbor = 0;

    if ((x1 == x2) && (y1 - 1 == y2)) neighbor++;
    if (rep(x2, y2).left == rep(x1, y1 - 1 + neighbor).right) f2++;
    neighbor = 0;

    if ((x1 - 1 == x2) && (y1 == y2)) neighbor++;
    if (rep(x2, y2).up == rep(x1 - 1 + neighbor, y1).down) f2++;
    neighbor = 0;

    if ((x1 == x2) && (y1 + 1 == y2)) neighbor++;
    if (rep(x2, y2).right == rep(x1, y1 + 1 - neighbor).left) f2++;
    neighbor = 0;

    if ((x1 + 1 == x2) && (y1 == y2)) neighbor++;
    if (rep(x2, y2).down == rep(x1 + 1 - neighbor, y1).up) f2++;
    neighbor = 0;

    // Calculus for the slot (x2, y2)
    int g1 = 0;
    if ((rep(x2, y2).left == rep(x2, y2 - 1).right) &&
        !((x2 == x1) && ((y2 - 1) == y1)))
      g1++;
    if ((rep(x2, y2).up == rep(x2 - 1, y2).down) &&
        !(((x2 - 1) == x1) && (y2 == y1)))
      g1++;
    if ((rep(x2, y2).right == rep(x2, y2 + 1).left) &&
        !((x2 == x1) && ((y2 + 1) == y1)))
      g1++;
    if ((rep(x2, y2).down == rep(x2 + 1, y2).up) &&
        !(((x2 + 1) == x1) && (y2 == y1)))
      g1++;

    int g2 = 0;
    if ((rep(x1, y1).left == rep(x2, y2 - 1).right) &&
        !((x2 == x1) && ((y2 - 1) == y1)))
      g2++;
    if ((rep(x1, y1).up == rep(x2 - 1, y2).down) &&
        !(((x2 - 1) == x1) && (y2 == y1)))
      g2++;
    if ((rep(x1, y1).right == rep(x2, y2 + 1).left) &&
        !((x2 == x1) && ((y2 + 1) == y1)))
      g2++;
    if ((rep(x1, y1).down == rep(x2 + 1, y2).up) &&
        !(((x2 + 1) == x1) && (y2 == y1)))
      g2++;

    double f = (f2 - f1) + (g2 - g1);

    // return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static std::string idComponent() {
    string idComp = MoveSwapCenter::idComponent();
    idComp.append(":DeltaMoveSwapCenter");
    return idComp;
  }

  virtual bool operator==(const DeltaMoveSwapCenter& _m) const {
    const DeltaMoveSwapCenter& m = (const DeltaMoveSwapCenter&)_m;
    return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
  }

  void print() const override {
    std::cout << "DeltaMoveSwapCenter: (" << x1 << "," << y1 << ") <=> (" << x2
              << "," << y2 << ")" << std::endl;
  }

  std::string id() const override {
    return "OptFrame:Move:DeltaMoveSwapCenter";
  }
};
}  // namespace EtII
#endif /*EtII_Delta_Move_SwapCenter_HPP_*/
