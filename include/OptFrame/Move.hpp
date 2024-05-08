// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOVE_HPP_  // NOLINT
#define OPTFRAME_MOVE_HPP_

// C++
#include <string>
//

#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>

namespace optframe {

// Move: provides move operator over XSH type (XES-based type)
// Note that, for XSH, we envision possible IncumbentType like EPopulation or
// EPareto applications, which is not existing for the moment (only BestType
// XES-based move operators)
//
// Future will tell if this was indeed necessary.

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class Move : public Component {
  using XEv = typename XES::second_type;
  using mvObjType = typename XEv::objType;

 public:
  ~Move() override = default;

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

  /*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual Move<XES, XSH>* applyMEV(MultiEvaluation<>& mev, XES& s)
   {
      // boolean 'outdated' indicates that Evaluation needs update (after
   Solution change)
      // note that even if the reverse move is applied, the Evaluation will
   continue with
      // the outdated status set to true. So more efficient approaches may
   rewrite this
      // method, or implement efficient re-evaluation by means of the 'cost'
   method. for (unsigned nE = 0; nE < mev.size(); nE++) mev.setOutdated(nE,
   true);
      // apply the move to R and ADS, saving the reverse (or undo) move
      Move<XES, XSH>* rev = apply(s);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(s);
      // return reverse move (or null)
      return rev;
   }
*/

  /*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual uptr<Move<XES, XSH>> applyMEVUpdate(MultiEvaluation<>& mev, XES&
   se)
   {
      for (unsigned nE = 0; nE < mev.size(); nE++)
         mev.at(nE).outdated = true;
      // apply the move to R and ADS, saving the reverse (or undo) move
      uptr<Move<XES, XSH>> rev = apply(se);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(se);

      // return reverse move (or null)
      return rev;
   }
*/

  // TODO: coming in one of the next versions..
  // virtual pair<Move<XES, XSH>&, MoveCost<>*> apply(const Evaluation<>&
  // e, R& r, ADS& ads) = 0;

  // ================== cost calculation

  // Returns a XEvaluation object containing the difference
  virtual op<XEv> cost(const XES& se, bool allowEstimated) {
    return std::nullopt;
  }

  // experiment for multi objective problems
  // ABANDONED! MUST SUPPORT SAME 'cost()' OF SINGLE-OBJ
  /*
  virtual MultiMoveCost<mvObjType>* costMEV(const MultiEvaluation<mvObjType>&
  mev, const XES& se, bool allowEstimated) { return nullptr;
  }
  */

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

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":Move";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }

  // default NO PRINT (WHY??)
  void print() const override { (*this->logdata) << toString() << std::endl; }
};

}  // namespace optframe

#endif  // OPTFRAME_MOVE_HPP_ // NOLINT
