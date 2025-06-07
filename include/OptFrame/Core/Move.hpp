// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOVE_HPP_  // NOLINT
#define OPTFRAME_MOVE_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <string>
//

#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>

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

namespace optframe {

// Move: provides move operator over XSH type (XES-based type)
// Note that, for XSH, we envision possible IncumbentType like EPopulation or
// EPareto applications, which is not existing for the moment (only BestType
// XES-based move operators)
// In this case, XES is Base Type, and XSH is Primary type.
// Example: XES and XSH=Pareto<XES>.
// Should move occur on base type or only in primary type?
//
// Future will tell if this was indeed necessary.

// ============== Is Move a Component? =================
// defining move as component brings larger costs...
// do we want that?
// - If defined, then Move id() is: "OptFrame:Move"
// - Otherwise, its id() hierarchy begins on move: "Move"
//
// #define MOVE_IS_COMPONENT
//
// =====================================================

#ifdef MOVE_IS_COMPONENT
#warning "Move is Component!"
#define MOVE_COMPONENT_OVERRIDE override
#else
#define MOVE_COMPONENT_OVERRIDE
#endif

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class Move
#ifdef MOVE_IS_COMPONENT
    : public Component
#endif
{
  using XEv = typename XES::second_type;
  using mvObjType = typename XEv::objType;

 public:
  virtual ~Move() MOVE_COMPONENT_OVERRIDE = default;

  // move id
  virtual id_type mid() { return 0; }

  // default is ALL CAN BE APPLIED
  virtual bool canBeApplied(const XES& se) { return true; }

  // returns true if the apply returns a non-null pointer
  virtual bool hasReverse() {
    return true;  // TODO: make it pure virtual "= 0"
  }

  // apply move directly to solution structure (only XSolution required)
  virtual uptr<Move<XES, XSH>> apply(XSH& se) = 0;

  virtual void beforeApply(XEv& e) {
    // bool 'outdated' indicates that Evaluation needs update (after Solution
    // change) note that even if the reverse move is applied, the Evaluation
    // will continue with the outdated status set to true. So more efficient
    // approaches may rewrite this method, or use  efficient re-evaluation by
    // means of the 'cost' method.
    //
    // e.outdated = true; // IGNORE
    //
    // general case: for single and multi evaluations
    if (!e.isOutdated()) e.invalidate();
  }

  // apply move to solution structure and updated objective space component
  // (XSolution and XEvaluation)
  virtual uptr<Move<XES, XSH>> applyUpdate(XSH& se) {
    // invalidate evaluation before apply
    beforeApply(se.second);
    //
    // apply the move to R and ADS, saving the reverse (or undo) move
    uptr<Move<XES, XSH>> rev = apply(se);
    // update neighborhood local optimum status TODO:deprecated
    updateNeighStatus(se);

    // return reverse move (or null)
    return rev;
  }

  // ================== cost calculation

  // Returns a XEvaluation object containing the difference
  virtual op<XEv> cost(const XES& se, bool allowEstimated) {
    return std::nullopt;
  }

  // ================== move independence and local search marking

  virtual bool independentOf(const Move<XES, XSH>& m) {
    // example: in VRP, move1 changes one route and move2 changes another...
    // they are independent. move1.isIndependent(move2) should return true. by
    // default, it is false (no move is independent)
    return false;
  }

  // ================== local search marking

  // TODO: deprecated. replaced by updateLOS?
  // virtual void updateNeighStatus(ADS* ads)
  virtual void updateNeighStatus(XSH& se) {}

  // TODO: force before apply(R,ADS) and after apply(S)?
  // TODO: think about how this fits a general 'XR' structure... maybe better on
  // 'XEv' than 'XR' itself.
  virtual void updateLOS(XSH& s, XEv& e) {}

  // TODO: rethink!
  virtual bool isPartialLocalOptimum(const XES& se) {
    // the idea is to use this flag to ignore moves that are useless,
    // given that the solution is already in a (full) local optimum (or
    // partial).

    return false;
  }

  // TODO: do we need clone() here?
  // It may be useful (and necessary!) to handle multiple applications of same
  // Move... let's see!

  // ================== basic comparison

  // default is ALL DIFFERENT
  virtual bool operator==(const Move<XES, XSH>& m) const { return false; }

  bool operator!=(const Move<XES, XSH>& m) const { return !(*this == m); }

  // support basic debugging
#ifndef MOVE_IS_COMPONENT
  modlog::LogLevel verboseLevel;

  virtual void setMoveVerbose() { verboseLevel = modlog::LogLevel::Debug; }
#else
  virtual void setMoveVerbose() { this->setVerbose(); }
#endif

  static std::string idMoveComponent() {
    std::stringstream ss;
#ifdef MOVE_IS_COMPONENT
    ss << Component::idComponent() << ":";
#endif
    ss << "Move";
    return ss.str();
  }

#ifdef MOVE_IS_COMPONENT
  static std::string idComponent() { return idMoveComponent(); }
#endif

  virtual std::string id() const MOVE_COMPONENT_OVERRIDE {
    return idMoveComponent();
  }

  virtual std::string toString() const MOVE_COMPONENT_OVERRIDE { return id(); }

  // TODO: remove print()? forcing std::cout is bad idea... use toStream()!
  // [[deprecated]]
  virtual void print() const MOVE_COMPONENT_OVERRIDE {
    std::cout << toString() << std::endl;
  }

  virtual bool toStream(std::ostream& os) const MOVE_COMPONENT_OVERRIDE {
    os << toString();
    return true;  // returns 'false' if unsupported
  }
};

}  // namespace optframe

#endif  // OPTFRAME_MOVE_HPP_ // NOLINT
