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

#ifndef OPTFRAME_MOVE_HPP_
#define OPTFRAME_MOVE_HPP_

// C++
#include <string>
//

#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Component.hpp>
// #include "Evaluation.hpp"
// #include "MoveCost.hpp"
// #include "MultiEvaluation.hpp"
// #include "MultiMoveCost.hpp"
// #include "Solution.hpp"
// #include "Solutions/CopySolution.hpp"

// #include "Action.hpp"

// using namespace std;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe {

// it used to be a 'solution space' here:
// not anymore: template<XSolution S, XEvaluation XEv = Evaluation<>>
// Now, 'XR' is a representation space (free of constraints),
// which can be considered equivalent to the "solution space" (in some cases).
// This is useful to remove all possible overheads (including clone(),
// toString()) from a very basic structure 'XR' (if user wants to). This is also
// useful when solution is composed by smaller "parts", and each part operates
// with independent moves (thus a fraction of real "solution space"). Efficient
// components (like Move) should use 'XR' instead of 'XSolution' (and
// equivalents).
// template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH =
// std::pair<S, XEv> > template<XSolution S, XEvaluation XEv = Evaluation<>,
// XESolution XSH = std::pair<S, XEv>>
template <XESolution XES, XEvaluation XEv = typename XES::second_type,
          XSearch<XES> XSH = XES>
// BREAK TIME!! ONLY 'XES' shall pass... can we finish 'S' here?
// Finally, now it's possible to abolish XEv (XEv2) passing here, and deduce
// from 'XES::second_type'.
class Move : public Component {
  // using XEv = decltype(declval<XSH>.second); // error: insufficient
  // contextual information to determine type using XEv = typename
  // XES::second_type; // This works!!! But better done directly on template
  // header TOO BAD: we really need to pass XEv here

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
  virtual uptr<Move<XES, XEv, XSH>> apply(XSH& se) = 0;

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
  virtual uptr<Move<XES, XEv, XSH>> applyUpdate(XSH& se) {
    // invalidate evaluation before apply
    beforeApply(se.second);
    //
    // apply the move to R and ADS, saving the reverse (or undo) move
    uptr<Move<XES, XEv, XSH>> rev = apply(se);
    // update neighborhood local optimum status TODO:deprecated
    updateNeighStatus(se);

    // return reverse move (or null)
    return rev;
  }

  /*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual Move<XES, XEv, XSH>* applyMEV(MultiEvaluation<>& mev, XES& s)
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
      Move<XES, XEv, XSH>* rev = apply(s);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(s);
      // return reverse move (or null)
      return rev;
   }
*/

  /*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual uptr<Move<XES, XEv, XSH>> applyMEVUpdate(MultiEvaluation<>& mev, XES&
   se)
   {
      for (unsigned nE = 0; nE < mev.size(); nE++)
         mev.at(nE).outdated = true;
      // apply the move to R and ADS, saving the reverse (or undo) move
      uptr<Move<XES, XEv, XSH>> rev = apply(se);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(se);

      // return reverse move (or null)
      return rev;
   }
*/

  // TODO: coming in one of the next versions..
  // virtual pair<Move<XES, XEv, XSH>&, MoveCost<>*> apply(const Evaluation<>&
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

  virtual bool independentOf(const Move<XES, XEv, XSH>& m) {
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
  virtual bool operator==(const Move<XES, XEv, XSH>& m) const { return false; }

  bool operator!=(const Move<XES, XEv, XSH>& m) const { return !(*this == m); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":Move";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }

  // default NO PRINT (WHY??)
  void print() const override { std::cout << toString() << std::endl; }
};

}  // namespace optframe

#endif  // OPTFRAME_MOVE_HPP_
