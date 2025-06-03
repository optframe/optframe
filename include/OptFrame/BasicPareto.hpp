// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_BASICPARETO_HPP_
#define OPTFRAME_BASICPARETO_HPP_

// BasicPareto is meant to be a simplification of Pareto class

// C++
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>
// #include <OptFrame/Hyper/ComponentBuilder.hpp>

namespace optframe {

// ========================================================
// remember that XMES = (S, [E])
// BasicPareto is a X2MES implementation, or simply, 2^XMES
//
// WORKAROUND FOR XES... SAME AS PARETO!
//
template <XEMSolution XMES>
class BasicPareto {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
  static_assert(XEvaluation<typename XMEv::XEv>);
#endif
  // static_assert(std::is_same_v<XMEv, MultiEvaluation<>>);
  using XEv = typename XMEv::XEv;
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
  static_assert(XEvaluation<XEv>);
#endif

 public:
  using value_type = XMES;

 private:
  // all individuals are safely stored here
  vector<uptr<XMES>> paretoSetFront;
  // these are just mirrors of stored set above
  vector<S*> paretoSet;
  vector<XMEv*> paretoFront;

 public:
  BasicPareto() {}

  BasicPareto(const BasicPareto<XMES>& _pf) {
    for (unsigned i = 0; i < _pf.paretoSetFront.size(); i++) {
      // NOLINTNEXTLINE
      auto* se_ptr = new XMES{*_pf.paretoSetFront[i]};
      paretoSet.push_back(&se_ptr->first);
      paretoFront.push_back(&se_ptr->second);
      paretoSetFront.push_back(uptr<XMES>{se_ptr});
    }
  }

  BasicPareto(BasicPareto<XMES>&& _pf) noexcept
      : paretoSetFront(std::move(_pf.paretoSetFront)),
        paretoSet{std::move(_pf.paretoSet)},
        paretoFront{std::move(_pf.paretoFront)} {}

  BasicPareto<XMES>& operator=(const BasicPareto<XMES>& _pf) {
    if (&_pf == this)  // auto ref check
      return *this;
    //
    this->clear();
    //
    for (unsigned i = 0; i < _pf.paretoSetFront.size(); i++) {
      auto* se_ptr = new XMES{*_pf.paretoSetFront[i]};
      this->paretoSet.push_back(&se_ptr->first);
      this->paretoFront.push_back(&se_ptr->second);
      this->paretoSetFront.push_back(uptr<XMES>{se_ptr});
    }

    return (*this);
  }

  virtual ~BasicPareto() { this->clear(); }

  void push_back(const XMES& se) {
    // NOLINTNEXTLINE
    XMES* u_se_ptr = new XMES{se};
    // data is divided into two auxiliary vectors (why?)
    paretoSet.push_back(&u_se_ptr->first);
    paretoFront.push_back(&u_se_ptr->second);
    // data is safely stored on paretoSetFront
    paretoSetFront.push_back(uptr<XMES>(u_se_ptr));
  }

  void add_indWithMev(const XMES& smev) {
    paretoSetFront.push_back(uptr<XMES>(new XMES(smev)));
  }

  unsigned size() const {
    // return paretoSet.size();
    return paretoSetFront.size();
  }

  const XMES& getP(unsigned index) const { return *paretoSetFront.at(index); }

  XMES& getP(unsigned index) { return *paretoSetFront.at(index); }

  // compatibility with X2ESolution
  XMES& at(unsigned index) { return getP(index); }

  // return "observer pointers" mirroring internal pareto set / front
  vector<XMES*> getParetoSetFrontPtr() {
    // mirror raw pointers (observers)
    // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
    vector<XMES*> paretoSetFrontPtr(paretoSetFront.size(), nullptr);
    for (unsigned i = 0; i < paretoSetFront.size(); i++)
      paretoSetFrontPtr[i] = paretoSetFront[i].get();
    //
    return paretoSetFrontPtr;
  }

  // return "observer pointers" mirroring internal pareto front
  vector<XMEv*> getParetoFrontPtr() {
    // mirror raw pointers (observers)
    // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
    vector<XMEv*> paretoFrontPtr(paretoSetFront.size(), nullptr);
    for (unsigned i = 0; i < paretoSetFront.size(); i++)
      paretoFrontPtr[i] = &paretoSetFront[i]->second;
    //
    return paretoFrontPtr;
  }

  S& getNonDominatedSol(int ind) { return paretoSetFront[ind]->first; }

  XMEv& getIndMultiEvaluation(int ind) const {
    return paretoSetFront.at(ind)->second;
  }

  void erase(unsigned pos) {
    paretoSetFront.erase(paretoSetFront.begin() + pos);
  }

  void clear() { paretoSetFront.clear(); }

  void print() {
    std::cout << "Printing BasicPareto!" << std::endl;
    std::cout << "paretoSetFront.size():" << paretoSetFront.size() << std::endl;
    for (unsigned i = 0; i < paretoSetFront.size(); i++) {
      std::cout << paretoSetFront[i]->first << std::endl;
      paretoSetFront[i]->second.print();
    }
  }
};

}  // namespace optframe

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
namespace optframe {
using ParetoSolTest0 = std::vector<int>;
using EMSolution_Test0 = std::pair<ParetoSolTest0, MultiEvaluation<double>>;
// compilation tests
static_assert(X2ESolution<BasicPareto<EMSolution_Test0>, EMSolution_Test0>);
static_assert(XEMSolution<EMSolution_Test0>);
static_assert(X2EMSolution<BasicPareto<EMSolution_Test0>, EMSolution_Test0>);

}  // namespace optframe
#endif  // cpp_concepts

#endif  // OPTFRAME_BASICPARETO_HPP_
