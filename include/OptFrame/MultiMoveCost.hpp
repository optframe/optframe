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

#ifndef OPTFRAME_MULTIMOVECOST_HPP_
#define OPTFRAME_MULTIMOVECOST_HPP_

// DISABLE
#ifdef FALSE

#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Component.hpp"
#include "Evaluation.hpp"
#include "MoveCost.hpp"

using namespace std;

namespace optframe {

//// more than 'objval' we need to ensure arithmetics here... TODO: see that
///(same as Evaluation)
template <optframe::objval ObjType = evtype,
          XEvaluation XEv = Evaluation<ObjType>>
// template<class ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
class MultiMoveCost : public Component {
 protected:
  vector<MoveCost<ObjType, XEv>*> vmc;

 public:
  explicit MultiMoveCost(std::vector<MoveCost<>*> _vmc) : vmc(_vmc) {}

  MultiMoveCost(const MultiMoveCost<>& mc) : vmc(mc.vmc) {}

  virtual ~MultiMoveCost() {}

  int size() const { return vmc.size(); }

  bool hasCost(int k) const { return vmc[k]; }

  bool isEstimated(int k) const { return vmc[k]->estimated; }

  const std::vector<std::pair<ObjType, ObjType>>& getAlternativeCosts(int k) const {
    return vmc[k]->alternatives;
  }

  ObjType getObjFunctionCost(int k) const { return vmc[k]->objFunction; }

  ObjType getInfMeasureCost(int k) const { return vmc[k]->infMeasure; }

  void addAlternativeCost(const pair<ObjType, ObjType>& alternativeCost,
                          int k) {
    vmc[k]->alternatives.push_back(alternativeCost);
  }

  void setAlternativeCosts(
      const std::vector<std::pair<ObjType, ObjType>>& alternativeCosts, int k) {
    vmc[k]->alternatives = alternativeCosts;
  }

  void setObjFunctionCost(ObjType obj, int k) { vmc[k]->objFunction = obj; }

  void setInfMeasureCost(ObjType inf, int k) { vmc[k]->infMeasure = inf; }

  ObjType cost(int k) const { return vmc[k]->cost(); }

  static std::string idComponent() { return "OptFrame:MultiMoveCost"; }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  void print() const override {
    std::cout << fixed;  // disable scientific notation
    std::cout << "MultiMoveCost for " << size() << " objectives:" << std::endl;
    for (unsigned i = 0; i < vmc.size(); i++)
      if (vmc[i])
        vmc[i]->print();
      else
        std::cout << "NO COST" << std::endl;
  }

  virtual MultiMoveCost<>& operator=(const MultiMoveCost<>& mmc) {
    if (&mmc == this)  // auto ref check
      return *this;

    vmc = mmc.vmc;  // TODO fix: this should handle some local instances, for
                    // the future...

    return *this;
  }

  virtual MultiMoveCost<>& clone() const { return *new MultiMoveCost<>(*this); }
};

#ifndef NDEBUG
struct optframe_debug_test_multimove_cost {
  MultiMoveCost<> testMoveCost;
};
#endif

}  // namespace optframe

#endif  // DISABLED

#endif  // OPTFRAME_MULTIMOVECOST_HPP_
