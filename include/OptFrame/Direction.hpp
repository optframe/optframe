// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DIRECTION_HPP_  // NOLINT
#define OPTFRAME_DIRECTION_HPP_  // NOLINT

// C
#include <assert.h>
#include <float.h>
// C++
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <utility>
//
// Base concepts
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/MoveCost.hpp>

namespace optframe {

// Direction 'betterThan' depends on a 'totally_ordered' type XEv::objType
template <XEvaluation XEv>
class Direction : public Component {
  using objType = typename XEv::objType;

 public:
  /// MoveCost<evtype, Evaluation<>> nullCost;
  XEv nullCost;
  // worst possible value
  std::optional<objType> opNadir;
  // best possible value
  std::optional<objType> opIdeal;
  // minimum value
  std::optional<objType> opMin;
  // maximum value
  std::optional<objType> opMax;

  // nullCost(MoveCost<>(0))
  // requires cast from value zero, at least
  // TODO: or... simply assume that "empty" means "zero"
  Direction() : nullCost(XEv(0)) {}

  ~Direction() override = default;

  // ============ betterThan ===========

  //! abstract method betterThan: true when a < b for minimization problems; and
  //! true when a > b for maximization problems.
  /*!
         betterThan is the method in OptFrame used to guide the search methods
     into the solution space. with betterThan the search methods are able to
     compare good and poor solutions, in one of the two directions: minimization
     and maximization. It must be implemented by the final user in one of these
     ways:
         - for minimization problems, returns a < b;
         - for maximization problems, returns a > b.
         */

  // true if 'mc1' is better than 'mc2'
  virtual bool betterThan(const MoveCost<XEv>& mc1, const MoveCost<XEv>& mc2) {
    if (isMinimization())
      return (mc2.cost() - mc1.cost()) >=
             optframe::num_zero<typename XEv::objType>();
    else
      return (mc1.cost() - mc2.cost()) >=
             optframe::num_zero<typename XEv::objType>();
  }

  // true if 'e1' is (strictly) better than 'e2'
  virtual inline bool betterThan(const XEv& e1, const XEv& e2) {
    evtype diff;
    if (isMinimization())
      diff = e2.evaluation() - e1.evaluation();
    else
      diff = e1.evaluation() - e2.evaluation();
    // check if it's strictly better
    bool r = !(optframe::numeric_is_zero<typename XEv::objType>(diff)) &&
             (diff > optframe::num_zero<typename XEv::objType>());
    return r;
  }

  virtual bool equals(const XEv& e1, const XEv& e2) {
    return equals(e1.evaluation(), e2.evaluation());
  }

  virtual bool equals(const typename XEv::objType& t1,
                      const typename XEv::objType& t2) {
    if (optframe::numeric_is_zero<typename XEv::objType>(t1 - t2)) return true;
    // should not pass != operator
    if (t1 != t2) return false;
    // default case
    return true;
  }

  bool betterOrEquals(const XEv& e1, const XEv& e2) {
    return betterThan(e1, e2) || equals(e1, e2);
  }

  // ============= improvement =============

  // Analyse if '(mc + e1)' is an improvement over 'e2'
  virtual bool isImprovement(const XEv& mc, const XEv& e1, const XEv& e2) {
    evtype ec1 = mc.evaluation() + e1.evaluation();
    // minimization case
    if (isMinimization() &&
        (e2.evaluation() - ec1) >= optframe::num_zero<typename XEv::objType>())
      return true;
    // maximization case
    if (!isMinimization() &&
        (ec1 - e2.evaluation()) >= optframe::num_zero<typename XEv::objType>())
      return true;
    // default case
    return false;
  }

  virtual inline bool isImprovement(const XEv& mc) {
    return betterThan(mc, nullCost);
  }

 public:
  // ============= Direction ==============

  virtual bool isMinimization() const = 0;

  bool isMaximization() const { return !isMinimization(); }

  // ============ Limits =============

  // best possible value: caching opIdeal
  virtual objType ideal() {
    if (!opIdeal) {
      if (isMinimization())
        opIdeal = dmin();
      else
        opIdeal = dmax();
    }
    return *opIdeal;
  }

  //  worst possible value: caching opNadir
  virtual objType nadir() {
    if (!opNadir) {
      if (isMinimization())
        opNadir = dmax();
      else
        opNadir = dmin();
    }
    return *opNadir;
  }

  // set min possible value: caching opMax
  // do not use 'min' as it breaks on windows... using 'dmin'
  virtual objType dmin() {
    if (!opMin) {
      if (numeric_limits<objType>::has_infinity)
        opMin = -numeric_limits<objType>::infinity();
      else
        opMin = -numeric_limits<objType>::max();
    }
    return *opMin;
  }

  // set max possible value: caching opMax
  // do not use 'max' as it breaks on windows... using 'dmax'
  virtual objType dmax() {
    if (!opMax) {
      if (numeric_limits<objType>::has_infinity)
        opMax = numeric_limits<objType>::infinity();
      else
        opMax = numeric_limits<objType>::max();
    }
    return *opMax;
  }

  // pass min and max: return ideal and nadir
  virtual std::pair<objType, objType> setLimits(const objType& vMin,
                                                const objType& vMax) {
    opMin = vMin;
    opMax = vMax;
    opNadir = std::nullopt;
    opIdeal = std::nullopt;
    // methods nadir() and ideal() will set internal limits
    return {ideal(), nadir()};
  }

  // ============= Component ===============

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << "Direction";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    if (isMinimization())
      return "Direction(MIN)";
    else
      return "Direction(MAX)";
  }

  void print() const override { cout << toString() << endl; }
};

// Minimization implementation
template <XEvaluation XEv = Evaluation<evtype>>
class Minimization : public Direction<XEv> {
 public:
  virtual ~Minimization() {}
  bool isMinimization() const override { return true; }
};

// Maximization implementation
template <XEvaluation XEv = Evaluation<evtype>>
class Maximization : public Direction<XEv> {
 public:
  virtual ~Maximization() {}
  bool isMinimization() const override { return false; }
};

}  // namespace optframe

#endif  // OPTFRAME_DIRECTION_HPP_
