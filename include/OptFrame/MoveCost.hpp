// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_MOVECOST_HPP_
#define OPTFRAME_MOVECOST_HPP_

#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Component.hpp"
#include "Evaluation.hpp"

using namespace std;

namespace optframe {

// more than 'objval' we need to ensure arithmetics here... TODO: see that (same
// as Evaluation)
// template<optframe::objval ObjType = evtype, XEvaluation XEv =
// Evaluation<ObjType>>
template <XEvaluation XEv>
class MoveCost : public Component {
  using ObjType = typename XEv::objType;  // = evtype;

 protected:
  // objective function value (cost difference)
  ObjType objFunction;
  // infeasibility measure value (cost difference)
  ObjType infMeasure;
  // constant to mutiply infeasibility weight
  evtype weight;
  // flag indicating if Evaluation was outdated
  bool outdated;
  // flag indicating if cost is estimated
  bool estimated;
  // storing costs for lexicographic approaches
  std::vector<std::pair<evtype, evtype>> alternatives;

 public:
  explicit MoveCost(ObjType obj, ObjType inf = 0, evtype w = 1,
                    bool _outdated = true, bool _estimated = false)
      : objFunction(obj),
        infMeasure(inf),
        weight(w),
        outdated(_outdated),
        estimated(_estimated) {}

  MoveCost(const MoveCost& mc)
      : objFunction(mc.objFunction),
        infMeasure(mc.infMeasure),
        weight(mc.weight),
        outdated(mc.outdated),
        estimated(mc.estimated),
        alternatives(mc.alternatives) {}

  virtual ~MoveCost() {}

  bool isEstimated() const { return estimated; }

  const std::vector<std::pair<evtype, evtype>>& getAlternativeCosts() const {
    return alternatives;
  }

  ObjType getObjFunctionCost() const { return objFunction; }

  ObjType getInfMeasureCost() const { return infMeasure; }

  void addAlternativeCost(const pair<evtype, evtype>& alternativeCost) {
    alternatives.push_back(alternativeCost);
  }

  void setAlternativeCosts(
      const std::vector<std::pair<evtype, evtype>>& alternativeCosts) {
    alternatives = alternativeCosts;
  }

  void setObjFunctionCost(ObjType obj) { objFunction = obj; }

  void setInfMeasureCost(ObjType inf) { infMeasure = inf; }

  ObjType cost() const { return objFunction + weight * infMeasure; }

  // update Evaluation with costs
  virtual void update(XEv& e)  // TODO: put correct ObjType here
  {
    // update objective function value
    e.setObjFunction(e.getObjFunction() + objFunction);
    // update infeasibility measure value
    e.setInfMeasure(e.getInfMeasure() + infMeasure);
    // restore previous 'outdated' status, if Evaluation wasn't outdated before
    if (!outdated) e.outdated = outdated;

    // may also update lexicographic costs...
  }

  static std::string idComponent() { return "OptFrame:MoveCost"; }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  void print() const override {
    std::cout << fixed;  // disable scientific notation
    std::cout << "Move cost = " << cost() << std::endl;
    if (alternatives.size() > 0) {
      std::cout << "Alternative Costs: ";
      for (unsigned i = 0; i < alternatives.size(); i++)
        std::cout << "(" << alternatives[i].first << ";" << alternatives[i].second
             << ") ";
      std::cout << std::endl;
    }
  }

  virtual MoveCost& operator=(const MoveCost& mc) {
    if (&mc == this)  // auto ref check
      return *this;

    objFunction = mc.objFunction;
    infMeasure = mc.infMeasure;
    outdated = mc.outdated;
    estimated = mc.estimated;
    alternatives = mc.alternatives;
    weight = mc.weight;

    return *this;
  }

  virtual MoveCost& clone() const { return *new MoveCost(*this); }

  // TODO: THIS FRIEND WAS DESTROYING EVERYTHING!!
  // friend class MultiMoveCost; // TODO: remove! experiment for MO problems
};

#ifndef NDEBUG
struct optframe_debug_test_move_cost {
  MoveCost<Evaluation<>> testMoveCost;
};
#endif

}  // namespace optframe

#endif  // OPTFRAME_MOVECOST_HPP_
