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

#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

// C++
#include <iostream>
//
//#include "ADSManager.hpp"
//#include "Action.hpp"
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/GeneralEvaluator.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/IEvaluator.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/MoveCost.hpp>
//#include "Solution.hpp"

//#define OPTFRAME_EPSILON 0.0001

// using namespace std;
// using namespace scannerpp;

namespace optframe {

//! \english The Evaluator class is responsible for the attribution of objective values for each Solution \endenglish \portuguese A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution \endportuguese

/*!
 \english
 The Evaluator class is responsible for the attribution of objective values for each Solution
 This is done by the method evaluate().
 \endenglish

 \portuguese
 A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution.
 Isto é feito através do método evaluate().
 \endportuguese
 */

// TODO: use XEv here
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = pair<S, XEv>>
//template<XSolution S, XEvaluation XEv> // require explicitly XEv here...
//template<XESolution XES, XEvaluation XEv = Evaluation<>>

// Evaluation may need to be S dependent, while GeneralEvaluator is not.
//template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
template <XSolution S, XEvaluation XEv, XESolution XES = pair<S, XEv>>
class Evaluator : public GeneralEvaluator<XES, XEv, XES>,
                  public IEvaluator<XES> {
  static_assert(is_same<S, typename XES::first_type>::value);
  static_assert(is_same<XEv, typename XES::second_type>::value);

 public:
  // Evaluator HAS_A Direction, not IS_A Direction!
  // This (hopefully) fixes Diamond problem with GeneralEvaluator
  sref<Direction<XEv>> direction;

 protected:
  bool allowCosts;  // move.cost() is enabled or disabled for this Evaluator
  evtype weight;    // defaults to 1

 public:
  Evaluator(sref<Direction<XEv>> _direction, bool _allowCosts = true, evtype w = 1)
      : direction{_direction}, allowCosts{_allowCosts}, weight{w} {
  }

  virtual ~Evaluator() {
  }

  // OLD Direction... now embedded here as well
  virtual bool isMinimization() const {
    return direction->isMinimization();
  }

  bool getAllowCosts() {
    return allowCosts;
  }

  void setAllowCosts(bool _allowCosts) {
    allowCosts = _allowCosts;
  }

  // TODO: remove weight
  evtype getWeight() const {
    return weight;
  }

  // TODO: remove weight
  void setWeight(const evtype& w) {
    weight = w;
  }

 public:
  // too bad: we cannot ask for 'XSolution S&' here... maybe on c++20 "full concepts".
  // solution: forcing 'S' template passing here
  XEv evaluate(const S&) override = 0;

  // TODO: verify if 'e.outdated' must be required at all times, or just specific cases
  //virtual void reevaluate(XEv& e, const XES& s) override
  virtual void reevaluate(XES& se) override {
    S& s = se.first;
    XEv& e = se.second;
    if (e.isOutdated())
      e = evaluate(s);
    //e = evaluate(se);
  }

 public:
  // Apply movement without considering a previous XEv => Slower.
  // Return new XEv 'e'
  pair<uptr<Move<XES, XEv>>, XEv> applyMove(Move<XES, XEv>& m, XES& se) {
    // apply move and get reverse move
    uptr<Move<XES, XEv>> rev = m.apply(se);
    // for now, must be not nullptr
    assert(rev != nullptr);
    // TODO: include management for 'false' hasReverse()
    assert(m.hasReverse() && rev);
    XEv e = evaluate(se.first);
    //this->reevaluate(se);
    // create pair
    return pair<uptr<Move<XES, XEv>>, XEv>(std::move(rev), std::move(e));  // TODO: verify if 'e' is copied, but probably requires std::move
                                                                           //return make_pair(rev, evaluate(s));
  }

  // Movement cost based on reevaluation of 'e'
  //MoveCost<>* moveCost(XEv& e, Move<XES, XEv>& m, XES& s, bool allowEstimated = false)

  // Movement cost based on complete evaluation (only on CheckCommand)
  // USE ONLY FOR VALIDATION OF CODE! OTHERWISE, USE MoveCost<>(e, m, s)
  ///MoveCost<>* moveCostComplete(Move<XES, XEv>& m, XES& s, bool allowEstimated = false)
  op<XEv> moveCostComplete(Move<XES, XEv>& m, XES& se, bool allowEstimated = false) {
    // TODO: in the future, consider 'allowEstimated' parameter
    // TODO: in the future, consider 'e' and 's' as 'const', and use 'const_cast' to remove it.

    // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
    assert(m.hasReverse());

    pair<uptr<Move<XES, XEv>>, XEv> rev = applyMove(m, se);

    pair<uptr<Move<XES, XEv>>, XEv> ini = applyMove(*rev.first, se);

    // Difference: new - original

    evtype obj = rev.second.getObjFunction() - ini.second.getObjFunction();
    evtype inf = rev.second.getInfMeasure() - ini.second.getInfMeasure();

    /*
      vector<pair<evtype, evtype>> alternatives(rev.second.getAlternativeCosts().size());
      for (unsigned i = 0; i < alternatives.size(); i++) {
         alternatives[i].first = rev.second.getAlternativeCosts()[i].first - ini.second.getAlternativeCosts()[i].first;
         alternatives[i].second = rev.second.getAlternativeCosts()[i].second - ini.second.getAlternativeCosts()[i].second;
      }
      */

    ///MoveCost<>* p = new MoveCost<>(obj, inf);
    op<XEv> p = make_optional(XEv(obj, inf));

    //p->setAlternativeCosts(alternatives);

    //delete rev.first;
    //delete ini.first;

    return p;
  }

  // ============ betterThan ===========

  // From Direction:
  //virtual inline bool betterThan(const MoveCost<>& mc1, const MoveCost<>& mc2)
  //using direction->betterThan;

  // Note that these parameters are NON-CONST... so, they can be updated if necessary!
  virtual bool betterThan(XES& se1, XES& se2) {
    XEv& e1 = se1.second;
    XEv& e2 = se2.second;
    if (e1.isOutdated())
      e1 = evaluate(se1.first);
    //e1 = evaluate(se1);
    if (e2.isOutdated())
      e2 = evaluate(se2.first);
    //e2 = evaluate(se2);
    //
    //bool r = direction->betterThan(e1, e2);
    bool r = direction->betterThan(e1, e2);
    return r;
  }

  //! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
  /*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
  //virtual bool betterThan(evtype a, evtype b) = 0;
  virtual bool betterThan(const XES& s1, const XES& s2) {
    //XEv e1 = evaluate(s1);
    //XEv e2 = evaluate(s2);
    // TODO: verify if outdated is not set!!
    XEv e1 = evaluate(s1.first);
    XEv e2 = evaluate(s2.first);
    bool r = direction->betterThan(e1, e2);
    return r;
  }

  virtual bool betterThan(const XEv& e1, const XEv& e2) {
    return direction->betterThan(e1, e2);
  }

  // =======================================

  // this strictly better than parameter 'e' (for mini, 'this' < 'e')
  virtual bool betterStrict(const XEv& e1, const XEv& e2) override {
    assert(!e1.isOutdated());
    assert(!e2.isOutdated());
    return direction->betterThan(e1, e2);
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
  virtual bool isStrictImprovement(const XEv& e) override {
    return direction->isImprovement(e);
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
  virtual bool isNonStrictImprovement(const XEv& e) override {
    return isStrictImprovement(e) || direction->equals(e, direction->nullCost);
  }

  virtual bool equals(const XEv& e1, const XEv& e2) override {
    return direction->equals(e1, e2);
  }

  // ---------------------------------------

  // ============= Component ===============

  virtual string toString() const override {
    return id();
  }

  virtual bool compatible(string s) override {
    // forcing comparison here (with GeneralEvaluator) due to Multiple Inheritance
    // TODO: find better solution to this
    //return (s == idComponent()) || (direction->compatible(s)) || (s == "OptFrame:GeneralEvaluator"); //|| (GeneralEvaluator<XES, XEv, XES>::compatible(s));
    // FIXED!
    return (s == idComponent()) || (GeneralEvaluator<XES, XEv, XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    // ss << Component::idComponent() << ":Evaluator";
    ss << GeneralEvaluator<XES>::idComponent() << ":Evaluator";
    // TODO: this will require multiple idComponent()!! NOT ANYMORE (no diamonds...)
    //ss << "OptFrame:GeneralEvaluator:Direction:Evaluator"; // DEPRECATED!
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }

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

  XEv evaluate(const RSolution<R>& s) override {
    return evaluate(s.getR());
  }
};
}  // namespace optframe

#endif /*OPTFRAME_EVALUATOR_HPP_*/
