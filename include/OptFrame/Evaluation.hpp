// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_EVALUATION_HPP_
#define OPTFRAME_EVALUATION_HPP_

// C
#include <assert.h>
// C++
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Helper/MultiObjValue.hpp>  // inserting this beforehand.. who knows!!!
#include <OptFrame/SingleObjValue.hpp>  // basic value 'evtype' comes from here!

// using namespace std;

namespace optframe {

//! \english The Evaluation class is a container class for the objective
//! function value and the Memory structure M. \endenglish \portuguese A classe
//! Evaluation é uma classe contêiner para o valor da função objetivo e a
//! estrutura de Memória M. \endportuguese

/*!
 \english
 It is also possible to carry an infeasibility measure.
 The evaluation() method returns the sum of objFunction and infMeasure.
 \endenglish

 \portuguese
 Também é possível carregar uma medida de inviabilidade infMeasure.
 O método evaluation() retorna a soma da função objetivo objFunction e a
 infMeasure. \endportuguese
 */

//#include "Util/PackTypes.hpp"
//#define EVALUATION_TYPE PackTypes

// note: for multi-objective problems with distinct objective space types
// such as (int, evtype, long long) you can use PackTypes in Utils or overload
// manually each of the numeric operators +, -, *
// why this isn't a template????????????????
// found a solution using C++20 concepts: now it's an optional template :)

// TODO: this ObjType here can require more than 'totally_ordered',
// as it will require arithmetics (+,-,*) to calculate MoveCosts and
// Infeasibility Thus, we should use another concept here, composed (&&) with
// arithmetics. Good thing is that Evaluation class is generic for most uses...
// This is not the case for Pareto solutions, where composability may become
// more complex (MultiEvaluation)
// TODO: must see next how to deal with that (on Pareto side), but at least we
// have a very elegant solution now

// here comes the tricky part, 'totally_ordered' should be enough, but we endup
// needing arithmetics to compute costs
// template<optframe::totally_ordered ObjType = evtype>
// so we will proceed with basic arithmetics, +, - and *.
// this effectively discard 'string' and others (although comparable)

// THIS IS FOCUSED SINGLE DIRECTION PERSPECTIVE (TOTAL ORDER).
// SEE PARAM 'isMini'
//
// template <optframe::basic_arithmetics ObjType = evtype>
template <ConceptsBasicArithmetics ObjType = evtype>
class Evaluation final : public Component {
 public:
  // exporting 'objType' type, based on template 'ObjType'
  using objType = ObjType;
  //
 protected:
  ObjType objValZero;

  // ==== Objective Space type: pair<evtype, evtype> ====
  // objective function value (default = double)
  ObjType objFunction;
  // infeasibility measure value (default = double)
  ObjType infMeasure;
  // for lexicographic approaches, use these extra evaluation values
  // vector<pair<ObjType, ObjType>> alternatives;

 protected:
  // boolean field to indicate if Evaluation needs an update
  bool outdated{true};
  // boolean field to indicate if Evaluation value is an estimation (not exact)
  bool estimated{false};

 public:
  // is minimization (WHY???)
  // bool isMini{true};

  static constexpr int x{num_zero<int>()};

  // ======================================
  // begin canonical part

  // TODO(IGOR): maybe create empty Evaluation with numeric_zero on 'obj'

  explicit Evaluation(const ObjType& obj, const ObjType& inf,
                      const evtype& w = 1)
      : objFunction(obj), infMeasure(inf) {
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);
#endif

    optframe::numeric_zero(objValZero);

    // gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  // NOLINTNEXTLINE // no explicit here
  Evaluation(const ObjType& obj) : objFunction(obj) {
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);
#endif

    optframe::numeric_zero(objValZero);
    optframe::numeric_zero(infMeasure);

    // gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  explicit Evaluation() {
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);
#endif

    optframe::numeric_zero(objValZero);
    optframe::numeric_zero(objFunction);
    optframe::numeric_zero(infMeasure);

    // gos = gos_unknown;
    // outdated = false;
    outdated = true;
    estimated = false;
  }

  // never put 'explicit' here!!
  Evaluation(const Evaluation<ObjType>& e)
      : objFunction(e.objFunction),
        infMeasure(e.infMeasure)
        //, alternatives(e.alternatives)
        //, gos(e.gos)
        ,
        outdated(e.outdated),
        estimated(e.estimated) {
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);
#endif

    optframe::numeric_zero(objValZero);
  }

  ~Evaluation() override {}

  Evaluation<ObjType>& operator=(const Evaluation<ObjType>& e) {
    if (&e == this)  // auto ref check
      return *this;

    objFunction = e.objFunction;
    infMeasure = e.infMeasure;
    outdated = e.outdated;
    estimated = e.estimated;
    // alternatives = e.alternatives;
    // gos = e.gos;
    // isMini = e.isMini;

    return *this;
  }

  Evaluation<ObjType>& operator=(Evaluation<ObjType>&& e) noexcept {
    if (&e == this)  // auto ref check
      return *this;

    // steal from 'e'
    objFunction = e.objFunction;
    infMeasure = e.infMeasure;
    outdated = e.outdated;
    estimated = e.estimated;
    // alternatives = e.alternatives;
    // gos = e.gos;
    // isMini = e.isMini;

    return *this;
  }

  virtual Evaluation<ObjType>& clone() const { return *new Evaluation(*this); }

  // end canonical part
  // ======================================
  // begin Evaluation methods

  // getters/setters
  bool isOutdated() const { return outdated; }

  void invalidate() { outdated = true; }

  bool isEstimated() const { return estimated; }

  static constexpr ObjType getZero() {
    ObjType objValZero;
    optframe::numeric_zero(objValZero);
    return objValZero;
  }

  ObjType getObjFunction() const { return objFunction; }

  ObjType getInfMeasure() const { return infMeasure; }

  void setObjFunction(ObjType obj) {
    objFunction = obj;
    // I THINK this is right... but let's warn, for now!
    // Otherwise, where else could we set outdated to false??
    // if (Component::warning)
    //  std::cout << "WARNING: setObjFunction sets outdated=false" << std::endl;

    outdated = false;  // this is natural and should be allowed
  }

  void setInfMeasure(ObjType inf) { infMeasure = inf; }

  ObjType evaluation() const { return objFunction + infMeasure; }

  // ========= TAKEN FROM MoveCost =======

  // update target Evaluation with *this cost
  // this requires operator+ over ObjType
  virtual void update(Evaluation<ObjType>& evTarget) const {
    assert(!evTarget.isOutdated());  // NOLINT
    assert(!this->isOutdated());     // NOLINT
    evTarget = Evaluation<ObjType>{
        evTarget.getObjFunction() + this->getObjFunction(),
        evTarget.getInfMeasure() + this->getInfMeasure(), true};
  }

  // returns the difference/cost between evFinal - evThis
  virtual Evaluation<ObjType> diff(const Evaluation<ObjType>& evFinal) const {
    const Evaluation<ObjType>& e = evFinal;  // pointing to legacy code

    // take my own information
    // -----------------------

    pair<ObjType, ObjType> e_begin =
        make_pair(this->getObjFunction(), this->getInfMeasure());

    // -----------------------
    // compute cost difference
    // -----------------------
    Evaluation<ObjType> mcost(e.getObjFunction() - e_begin.first,
                              e.getInfMeasure() - e_begin.second,
                              1);  // no outdated or estimated

    // ======================================================
    // For alternative/lexicographic costs, see WLxEvaluation
    // ======================================================

    return Evaluation<ObjType>(mcost);
  }

  Evaluation& operator-=(const Evaluation& e) {
    objFunction -= e.objFunction;
    infMeasure -= e.infMeasure;
    return *this;
  }

  Evaluation& operator+=(const Evaluation& e) {
    objFunction += e.objFunction;
    infMeasure += e.infMeasure;
    return *this;
  }

  friend Evaluation operator-(Evaluation lhs, const Evaluation& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend Evaluation operator+(Evaluation lhs, const Evaluation& rhs) {
    lhs += rhs;
    return lhs;
  }

  // equality is supported here
  virtual bool equals(const Evaluation& e) {
    return evaluation() == e.evaluation();
  }

  bool operator==(const Evaluation& e) {
    return evaluation() == e.evaluation();
  }

  bool operator!=(const Evaluation& e) { return !(*this == e); }

  // ===============
  // finish MoveCost
  // ===============

  // leave option to rewrite tolerance (or consider lexicographic values)
  virtual bool isFeasible() const {
    return optframe::numeric_is_zero(infMeasure);
  }

  // ======================
  // Object specific part
  // ======================

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":Evaluation";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override { std::cout << toString() << endl; }

  std::string toString() const override {
    // ONE SHOULD NEVER PRINT AN EVALUATION WITH OUTDATED FLAG... SANITY CHECK!
    assert(!outdated);
    std::stringstream ss;
    ss << fixed;  // disable scientific notation
    ss << "Evaluation function value = " << evaluation();
    ss << (isFeasible() ? " " : " (not feasible) ");
    ss << (outdated ? " OUTDATED " : " ");
    ss << (estimated ? " ESTIMATED " : " ");
    return ss.str();
  }

  bool toStream(std::ostream& os) const override {
    // forward to operator<<
    os << (*this);
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const Evaluation& me) {
    if (&os == &optframe::cjson) {
      os << "{";
      os << "\"evaluation\": " << me.evaluation() << ",";
      os << "\"feasible\": " << (me.isFeasible() ? "true" : "false") << ",";
      os << "\"outdated\": " << (me.outdated ? "true" : "false") << ",";
      os << "\"estimated\": " << (me.estimated ? "true" : "false") << "}";
    } else {
      os << me.toString();
    }
    return os;
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
// compilation test (for concepts)
// debugging error on XEvaluation for IsEvaluation<int>
static_assert(optframe::evgoal<Evaluation<>>);
static_assert(optframe::basic_arithmetics<Evaluation<>>);
static_assert(HasClone<Evaluation<>>);
static_assert(HasToString<Evaluation<>>);
static_assert(HasGetObj<Evaluation<>>);
static_assert(optframe::ostreamable<Evaluation<>>);
//
static_assert(XEvaluation<Evaluation<>>);  // check default
#endif
// ==================== end Evaluation ===========

}  // namespace optframe

struct basic_ev_test_copy {
  void f() {
    optframe::Evaluation<> e1;
    optframe::Evaluation<> e2 = e1;  // test 'explicit'
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
// Compilation tests for XEvaluation concepts
// These are NOT unit tests... Unit Tests are on tests/ folder
#include <OptFrame/Evaluation.test.hpp>
#endif

#endif  // OPTFRAME_EVALUATION_HPP_
