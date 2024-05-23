// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIDIRECTION_HPP_
#define OPTFRAME_MULTIDIRECTION_HPP_

// C
#include <float.h>
// C++
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Move.hpp>
// #include <OptFrame/MoveCost.hpp>
//

namespace optframe {

// ===============================================
// this is homogeneous multi-obj with obj type XEv
// ===============================================

template <XEvaluation XEv>
class MultiDirection : public Component {
 protected:
  vsref<Direction<XEv>> vDir;

 public:
  uint32_t nObjectives;

  explicit MultiDirection(vsref<Direction<XEv>>& _vDir)
      : vDir{_vDir}, nObjectives{(uint32_t)vDir.size()} {}

  MultiDirection(MultiDirection& mDir)
      : vDir{mDir.vDir}, nObjectives{mDir.nObjectives} {}

  MultiDirection() : nObjectives{0} {}

  ~MultiDirection() override = default;

  virtual MultiDirection& addObjective(Direction<XEv>* ds) {
    if (ds) vDir.push_back(ds);
    nObjectives = vDir.size();

    return *this;
  }

  vsref<Direction<XEv>> getDirections() { return vDir; }

  // ============ betterThan ===========

  template <class T>
  inline bool betterThanAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->betterThan(a, b);
  }

  // ============ betterOrEquals ===========

  template <class T>
  inline bool betterOrEqualsAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->betterOrEquals(a, b);
  }

  // ============ equals ============

  template <class T>
  inline bool equalsAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->equals(a, b);
  }

  // ============= improvement =============

  /// inline bool isImprovement(unsigned obj, const MoveCost<>& mc, const
  /// Evaluation<>& e1, const Evaluation<>& e2)
  inline bool isImprovementAt(unsigned obj, const Evaluation<>& mc,
                              const Evaluation<>& e1, const Evaluation<>& e2) {
    return vDir[obj]->isImprovement(mc, e1, e2);
  }

  /// inline bool isImprovement(unsigned obj, const MoveCost<>& mc)
  inline bool isImprovementAt(unsigned obj, const Evaluation<>& mc) {
    return vDir[obj]->isImprovement(mc);
  }

  // ============= direction ==============

  inline bool isMinimizationAt(unsigned obj) {
    return vDir[obj]->isMinimization();
  }

  inline bool isMaximizationAt(unsigned obj) {
    return vDir[obj]->isMaximization();
  }

  // ============ estimation =============

  inline double ideal(unsigned obj) { return vDir[obj]->ideal(); }

  // inline double worst(unsigned obj)
  inline double nadir(unsigned obj) { return vDir[obj]->nadir(); }

  // do not use 'min' as it breaks on windows... using 'dmin'
  inline double dmin(unsigned obj) { return vDir[obj]->dmin(); }

  // do not use 'max' as it breaks on windows... using 'dmax'
  inline double dmax(unsigned obj) { return vDir[obj]->dmax(); }

  // ============= Component ===============

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  std::string toString() const override { return id(); }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":MultiDirection";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_MULTIDIRECTION_HPP_
