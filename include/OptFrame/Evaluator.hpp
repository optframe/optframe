// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

// C++
#include <iostream>
#include <string>
#include <utility>
//
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/GeneralEvaluator.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/ICompare.hpp>
#include <OptFrame/IEvaluator.hpp>
#include <OptFrame/Move.hpp>

namespace optframe {

//! \english The Evaluator class is responsible for the attribution of objective
//! values for each Solution \endenglish \portuguese A classe Evaluator é
//! responsável pela atribuição de valores objetivo para cada Solution
//! \endportuguese

/*!
 \english
 The Evaluator class is responsible for the attribution of objective values for
 each Solution This is done by the method evaluate(). \endenglish

 \portuguese
 A classe Evaluator é responsável pela atribuição de valores objetivo para cada
 Solution. Isto é feito através do método evaluate(). \endportuguese
 */

// TODO: use XEv here
// template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH =
// pair<S, XEv>> template<XSolution S, XEvaluation XEv> // require explicitly
// XEv here... template<XESolution XES, XEvaluation XEv = Evaluation<>>

// Evaluation may need to be S dependent, while GeneralEvaluator is not.
// template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES =
// pair<S, XEv>>
//
// TODO: adopt XES or XESS only
template <XSolution _S, XEvaluation _XEv, XESolution XES = pair<_S, _XEv>>
class Evaluator : public GeneralEvaluator<XES, XES>,
                  public IEvaluator<XES>,
                  public ICompare<typename XES::second_type> {
  using my_evtype = typename _XEv::objType;
  //
  // static_assert(is_same<S, typename XES::first_type>::value);
  // static_assert(is_same<XEv, typename XES::second_type>::value);
  //
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-type based structure (BestType always here)

 public:
  // Evaluator HAS_A Direction, not IS_A Direction!
  // This (hopefully) fixes Diamond problem with GeneralEvaluator
  sref<Direction<XEv>> direction;

 protected:
  bool allowCosts;  // move.cost() is enabled or disabled for this Evaluator
  evtype weight;    // defaults to 1

 public:
  explicit Evaluator(sref<Direction<XEv>> _direction, bool _allowCosts = true,
                     evtype w = 1)
      : direction{_direction}, allowCosts{_allowCosts}, weight{w} {}

  virtual ~Evaluator() = default;

  // OLD Direction... now embedded here as well
  virtual bool isMinimization() const { return direction->isMinimization(); }

  bool getAllowCosts() { return allowCosts; }

  void setAllowCosts(bool _allowCosts) { allowCosts = _allowCosts; }

  // TODO: remove weight
  evtype getWeight() const { return weight; }

  // TODO: remove weight
  void setWeight(const evtype& w) { weight = w; }

 public:
  // too bad: we cannot ask for 'XSolution S&' here... maybe on c++20 "full
  // concepts". solution: forcing 'S' template passing here
  XEv evaluate(const S&) override = 0;

  // TODO: verify if 'e.outdated' must be required at all times, or just
  // specific cases
  // virtual void reevaluate(XEv& e, const XES& s) override
  void reevaluate(XES& se) override {
    S& s = se.first;
    XEv& e = se.second;
    if (e.isOutdated()) e = evaluate(s);
    // e = evaluate(se);
  }

 public:
  // Apply movement without considering a previous XEv => Slower.
  // Return new XEv 'e'
  pair<uptr<Move<XES, XSH>>, XEv> applyMove(Move<XES, XSH>& m, XES& se) {
    // apply move and get reverse move
    uptr<Move<XES, XSH>> rev = m.apply(se);
    // for now, must be not nullptr
    assert(rev != nullptr);
    // TODO: include management for 'false' hasReverse()
    assert(m.hasReverse() && rev);
    XEv e = evaluate(se.first);
    // this->reevaluate(se);
    //  create pair
    return pair<uptr<Move<XES, XSH>>, XEv>(
        std::move(rev), std::move(e));  // TODO: verify if 'e' is copied, but
                                        // probably requires std::move
                                        // return make_pair(rev, evaluate(s));
  }

  // Movement cost based on reevaluation of 'e'
  // MoveCost<>* moveCost(XEv& e, Move<XES, XSH>& m, XES& s, bool allowEstimated
  // = false)

  // Movement cost based on complete evaluation (only on CheckCommand)
  // USE ONLY FOR VALIDATION OF CODE! OTHERWISE, USE MoveCost<>(e, m, s)
  /// MoveCost<>* moveCostComplete(Move<XES, XSH>& m, XES& s, bool
  /// allowEstimated = false)
  op<XEv> moveCostComplete(Move<XES, XSH>& m, XES& se,
                           bool allowEstimated = false) {
    // TODO: in the future, consider 'allowEstimated' parameter
    // TODO: in the future, consider 'e' and 's' as 'const', and use
    // 'const_cast' to remove it.

    // TODO: in the future, consider moves with nullptr reverse (must save
    // original solution/evaluation)
    assert(m.hasReverse());

    pair<uptr<Move<XES, XSH>>, XEv> rev = applyMove(m, se);

    pair<uptr<Move<XES, XSH>>, XEv> ini = applyMove(*rev.first, se);

    // Difference: new - original

    my_evtype obj = rev.second.getObjFunction() - ini.second.getObjFunction();
    my_evtype inf = rev.second.getInfMeasure() - ini.second.getInfMeasure();

    /*
      vector<pair<evtype, evtype>>
      alternatives(rev.second.getAlternativeCosts().size()); for (unsigned i =
      0; i < alternatives.size(); i++) { alternatives[i].first =
      rev.second.getAlternativeCosts()[i].first -
      ini.second.getAlternativeCosts()[i].first; alternatives[i].second =
      rev.second.getAlternativeCosts()[i].second -
      ini.second.getAlternativeCosts()[i].second;
      }
      */

    /// MoveCost<>* p = new MoveCost<>(obj, inf);
    op<XEv> p = std::make_optional(XEv{obj, inf});

    // p->setAlternativeCosts(alternatives);

    // delete rev.first;
    // delete ini.first;

    return p;
  }

  // ============ betterThan ===========

  // From Direction:
  // virtual inline bool betterThan(const MoveCost<>& mc1, const MoveCost<>&
  // mc2) using direction->betterThan;

  // Note that these parameters are NON-CONST... so, they can be updated if
  // necessary!
  virtual bool betterThan(XES& se1, XES& se2) {
    XEv& e1 = se1.second;
    XEv& e2 = se2.second;
    if (e1.isOutdated()) e1 = evaluate(se1.first);
    // e1 = evaluate(se1);
    if (e2.isOutdated()) e2 = evaluate(se2.first);
    // e2 = evaluate(se2);
    //
    // bool r = direction->betterThan(e1, e2);
    bool r = direction->betterThan(e1, e2);
    return r;
  }

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
  // virtual bool betterThan(evtype a, evtype b) = 0;
  virtual bool betterThan(const XES& s1, const XES& s2) {
    // XEv e1 = evaluate(s1);
    // XEv e2 = evaluate(s2);
    //  TODO: verify if outdated is not set!!
    XEv e1 = evaluate(s1.first);
    XEv e2 = evaluate(s2.first);
    bool r = direction->betterThan(e1, e2);
    return r;
  }

  bool betterThan(const XEv& e1, const XEv& e2) override {
    return direction->betterThan(e1, e2);
  }

  // =======================================

  // this strictly better than parameter 'e' (for mini, 'this' < 'e')
  bool betterStrict(const XEv& e1, const XEv& e2) override {
    assert(!e1.isOutdated());
    assert(!e2.isOutdated());
    return direction->betterThan(e1, e2);
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isStrictImprovement(const XEv& e) override {
    return direction->isImprovement(e);
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isNonStrictImprovement(const XEv& e) override {
    return isStrictImprovement(e) || direction->equals(e, direction->nullCost);
  }

  bool equals(const XEv& e1, const XEv& e2) override {
    return direction->equals(e1, e2);
  }

  // ---------------------------------------

  // ============= Component ===============

  string toString() const override { return id(); }

  bool compatible(std::string s) override {
    // forcing comparison here (with GeneralEvaluator) due to Multiple
    // Inheritance
    // TODO: find better solution to this
    // return (s == idComponent()) || (direction->compatible(s)) || (s ==
    // "OptFrame:GeneralEvaluator"); //|| (GeneralEvaluator<XES, XEv,
    // XES>::compatible(s));
    // FIXED!
    return (s == idComponent()) || (GeneralEvaluator<XES, XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    // ss << Component::idComponent() << ":Evaluator";
    ss << GeneralEvaluator<XES>::idComponent() << ":Evaluator"

       << Domain::getAlternativeDomain<XES>("<XESf64>");
    // TODO: this will require multiple idComponent()!! NOT ANYMORE (no
    // diamonds...)
    // ss << "OptFrame:GeneralEvaluator:Direction:Evaluator"; // DEPRECATED!
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  /*
   // id from GeneralEvaluator class (due to multiple inheritance)
   // TODO: REMOVE MULTIPLE INHERITANCE!!!!! VERY URGENT!!
   virtual std::string idGE() const override
   {
      return id();
   }
*/
};

template <XRepresentation R, XEvaluation XEv = Evaluation<>>
class BasicEvaluator : public Evaluator<RSolution<R>, XEv> {
 public:
  // only representation
  virtual XEv evaluate(const R&) = 0;

 private:
  /* Use this if don't need ADS */
  /* Use isto se você não precisa do ADS */

  using ADS = OPTFRAME_DEFAULT_ADS;

  XEv evaluate(const RSolution<R>& s) override { return evaluate(s.getR()); }
};
}  // namespace optframe

#endif /*OPTFRAME_EVALUATOR_HPP_*/
