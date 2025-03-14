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

#ifndef EtII_Delta_Move_Swap_Rotate_Center_HPP_
#define EtII_Delta_Move_Swap_Rotate_Center_HPP_

#include <cmath>

#include "NSSeqSwapRotateCenter.h"

using namespace std;

namespace EtII {

class DeltaMoveSwapRotateCenter : public MoveSwapRotateCenter {
 public:
  DeltaMoveSwapRotateCenter(int _x1, int _y1, int _x2, int _y2)
      : MoveSwapRotateCenter(_x1, _y1, _x2, _y2) {}

  virtual ~DeltaMoveSwapRotateCenter() {}

  op<Evaluation<>> cost(const pair<SolutionEtII, Evaluation<>>& se,
                        bool allowEstimated) override {
    double f = 0;

    // return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static std::string idComponent() {
    string idComp = MoveSwapRotateCenter::idComponent();
    idComp.append(":DeltaMoveSwapRotateCenter");
    return idComp;
  }

  virtual bool operator==(const DeltaMoveSwapRotateCenter& _m) const {
    const DeltaMoveSwapRotateCenter& m = (const DeltaMoveSwapRotateCenter&)_m;
    return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2) &&
           (m.r1 == r1) && (m.r2 == r2);
  }

  void print() const override {
    std::cout << "DeltaMoveSwapRotateCenter: (" << x1 << "," << y1 << ") r=" << r1
         << " <=> (" << x2 << "," << y2 << ") r=" << r2 << std::endl;
  }

  std::string id() const override {
    return "OptFrame:Move:DeltaMoveSwapRotateCenter";
  }
};
}  // namespace EtII
#endif /*EtII_Delta_Move_Swap_Rotate_Center_HPP_*/
