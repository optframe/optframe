// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIEVALUATOR_HPP_
#define OPTFRAME_MULTIEVALUATOR_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/IEvaluator.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Hyper/ComponentMultiBuilder.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>

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

// MultiEvaluator implements IEvaluator and GeneralEvaluator
// just a bunch/pack of evaluators...

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XEMSolution XMES, XSearch<XMES> XSH = XMES>
#else
MOD_EXPORT template <typename XES, typename XMES, typename XSH = XMES>
#endif
class MultiEvaluator : public GeneralEvaluator<XMES, XSH>,
                       public IEvaluator<XMES> {
  using S = typename XES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;

  using myObjType = typename XMEv::XEv::objType;

 public:
  // TODO(igormcoelho): why do we need this?
  //  -> Can't we just extract each Direction for each Evaluator?
  //
  // sref<MultiDirection<XEv>> mdirection;
  //
  vsref<Direction<XEv>> vDir;
  unsigned nObjectives;
  // ========== end old MultiDirection part ==========

 protected:
  // vector<Evaluator<S, XEv, XES>*> sngEvaluators;  // single evaluators
  //
  // single evaluators
  vsref<Evaluator<S, XEv, XES>> sngEvaluators;
  // move.cost() is enabled or disabled for this Evaluator
  bool allowCosts;

 public:
  // MultiEvaluator(std::vector<Evaluator<S, XEv, XES>*> _veval)
  explicit MultiEvaluator(const vsref<Evaluator<S, XEv, XES>>& _veval)
      : sngEvaluators{_veval}, allowCosts{false} {
    for (unsigned i = 0; i < _veval.size(); i++)
      vDir.push_back(_veval[i]->direction);
    nObjectives = vDir.size();
  }

  explicit MultiEvaluator(bool _allowCosts = false) : allowCosts(_allowCosts) {
    nObjectives = 0;
  }

  virtual void addEvaluator(sref<Evaluator<S, XEv, XES>> ev) {
    sngEvaluators.push_back(ev);
  }

  virtual ~MultiEvaluator() {}

  unsigned size() const { return sngEvaluators.size(); }

  virtual bool betterThanAt(const XEv& ev1, const XEv& ev2, int index) {
    return sngEvaluators[index]->betterThan(ev1, ev2);
  }

  virtual bool equalsAt(const XEv& ev1, const XEv& ev2, int index) {
    return sngEvaluators[index]->equals(ev1, ev2);
  }

  // changed to Meval without point TODO
  XMEv evaluate(const S& s) override {
    std::vector<XEv> vnev;
    for (unsigned i = 0; i < sngEvaluators.size(); i++) {
      XEv ev{sngEvaluators[i]->evaluate(s)};
      vnev.push_back(ev);
    }
    XMEv nev{vnev};

    return nev;
  }

  void clear() { sngEvaluators.clear(); }

  // virtual void reevaluateMEV(MultiXEv& mev, const XES& se)
  //
  // virtual void reevaluate(pair<S, MultiXEv>& se) override
  // virtual void reevaluate(pair<S, XMEv>& se) override
  void reevaluate(XMES& se) override {
    XMEv& mev = se.second;
    //
    for (unsigned i = 0; i < sngEvaluators.size(); i++) {
      // XEv e { std::move(mev[i]) }; // TODO (IGOR): why move????
      // sngEvaluators[i]->reevaluate(e, s);
      // mev[i] = std::move(e);
      //
      XEv& e = mev[i];  // TODO: embed MEV in 'se'

      std::pair<decltype(se.first), XEv> pse(
          se.first, e);  // TODO: we should AVOID this 's' and 'e' copy... by
                         // keeping s,e together.
      sngEvaluators[i]->reevaluate(pse);
      e = std::move(pse.second);  // TODO: verify if this works

      // mev[i] = std::move(e);
    }
  }

  // =======================

  // this strictly better than parameter 'e' (for mini, 'this' < 'e')
  bool betterStrict(const XMEv& e1, const XMEv& e2) override {
    assert(false);
    return false;
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isStrictImprovement(const XMEv& e) override {
    assert(false);
    return false;
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isNonStrictImprovement(const XMEv& e) override {
    assert(false);
    return false;
  }

  bool equals(const XMEv& e1, const XMEv& e2) override {
    assert(false);
    return false;
  }

  // ================================================

 public:
  // ============= Component ===============
  bool compatible(std::string s) override {
    return (s == idComponent()) || (GeneralEvaluator<XMES, XSH>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GeneralEvaluator<XMES, XSH>::idComponent() << ":MultiEvaluator"
       << Domain::getAlternativeDomain<XMES>("<XMESf64>");
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_MULTIEVALUATOR_HPP_
