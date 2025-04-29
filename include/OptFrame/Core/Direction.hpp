// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DIRECTION_HPP_  // NOLINT
#define OPTFRAME_DIRECTION_HPP_  // NOLINT

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

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
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/ICompare.hpp>
#include <OptFrame/Core/Move.hpp>
// #include <OptFrame/MoveCost.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

MOD_EXPORT namespace optframe {
  // Direction 'betterThan' depends on a 'totally_ordered' type XEv::objType
  template <XEvaluation XEv>
  class Direction : public Component, public ICompare<XEv> {
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

    // true if 'e1' is (strictly) better than 'e2'
    bool betterThan(const XEv& e1, const XEv& e2) override {
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
      if (optframe::numeric_is_zero<typename XEv::objType>(t1 - t2))
        return true;
      // should not pass != operator
      if (t1 != t2) return false;
      // default case
      return true;
    }

    virtual bool betterOrEquals(const XEv& e1, const XEv& e2) {
      return betterThan(e1, e2) || equals(e1, e2);
    }

    // ============= improvement =============

    // Analyse if '(mc + e1)' is an improvement over 'e2'
    virtual bool isImprovement(const XEv& mc, const XEv& e1, const XEv& e2) {
      evtype ec1 = mc.evaluation() + e1.evaluation();
      // minimization case
      if (isMinimization() && (e2.evaluation() - ec1) >=
                                  optframe::num_zero<typename XEv::objType>())
        return true;
      // maximization case
      if (!isMinimization() && (ec1 - e2.evaluation()) >=
                                   optframe::num_zero<typename XEv::objType>())
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
        // use 'limits' as variable, otherwise it breaks on windows...
        auto limits = std::numeric_limits<objType>();
        if (limits.has_infinity)
          opMin = -limits.infinity();
        else
          opMin = -limits.max();
      }
      return *opMin;
    }

    // set max possible value: caching opMax
    // do not use 'max' as it breaks on windows... using 'dmax'
    virtual objType dmax() {
      if (!opMax) {
        // use 'limits' as variable, otherwise it breaks on windows...
        auto limits = std::numeric_limits<objType>();
        if (limits.has_infinity)
          opMax = limits.infinity();
        else
          opMax = limits.max();
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

    static std::string idComponent() {
      std::stringstream ss;
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

    void print() const override { std::cout << toString() << std::endl; }
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

#endif  // OPTFRAME_CORE_DIRECTION_HPP_
