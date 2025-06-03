// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
#include <type_traits>  // static assert is_same
//
#include <OptFrame/Core/NSFind.hpp>
#include <OptFrame/Core/NSIterator.hpp>
#include <OptFrame/Experimental/NSBlockIterator.hpp>

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

// using namespace std;

namespace optframe {

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class NSSeq : public NSFind<XES, XSH> {
  using XEv = typename XES::second_type;
  using S = typename XES::first_type;

 public:
  virtual ~NSSeq() = default;

  uptr<Move<XES, XSH>> randomMove(const XES&) override = 0;

  virtual uptr<NSIterator<XES, XSH>> getIterator(const XES&) = 0;

  // experimental! Should create novel NSSeqBlock perhaps
  virtual uptr<NSBlockIterator<S, XEv, XES>> getBlockIterator(const XES& s) {
    uptr<NSIterator<XES>> it = this->getIterator(s);
    return uptr<NSBlockIterator<S, XEv, XES>>(
        new DefaultNSBlockIterator<S, XEv, XES>(*it));
  }

  // ============= For 'Local Optimum'-based methods =============

  // GET LOCAL OPTIMUM INFORMATION FROM SOLUTION (ADS PREFERED?)
  virtual LOS getLOS(const XES& s) { return los_unknown; }

  // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
  // MAYBE IT's BETTER TO USE ONLY IN ITERATORS! TODO: THINK ABOUT IT...
  // virtual void setLOS(LOS status, S& s)
  //{
  //}

 public:
  // =======================================
  // find section (neighborhood exploration)
  // =======================================

 private:
  // stateful iterator local variable
  uptr<NSIterator<XES, XSH>> it;

 public:
  // findFirst (from NSSeq): returns the *first* move that strictly improves
  // current solution 'se', according 'gev' RETURNS: std::pair< uptr<Move<XES,
  // XEv, XSH>>, op<XEv> > note that this method is *not stateless* regarding
  // NSSeq class, as a *stateful* iterator variable is locally stored
  virtual std::pair<Move<XES, XSH>*, op<XEv>> findFirst(
      GeneralEvaluator<XES, XSH>& gev, XES& se) {
    // initializes iterator
    it = this->getIterator(se);
    // finds first valid move
    it->firstValid(se);
    // gets current move (shall we test for isDone()?)
    uptr<Move<XES, XSH>> pm = it->current();
    // stores temporary raw pointer
    Move<XES, XSH>* m = pm.get();
    // gets cost for current move
    op<XEv> mvcost = gev.moveCost(*m, se);
    // if no cost, finishes
    if (!mvcost) return std::make_pair(nullptr, std::nullopt);
    // loops until finds improving move
    while (!gev.isStrictImprovement(*mvcost)) {
      // if done, finishes
      if (it->isDone()) return std::make_pair(nullptr, std::nullopt);
      it->nextValid(se);
      // gets current move (shall we test for isDone()?)
      pm = it->current();
      // stores temporary raw pointer
      m = pm.get();
      // gets cost for current move
      mvcost = gev.moveCost(*m, se);
      // if no cost, finishes
      if (!mvcost) return std::make_pair(nullptr, std::nullopt);
    }
    // current move should be improving
    return std::make_pair(pm.release(), mvcost);
  }

  // findNext (from NSSeq): returns the *next* move that strictly improves
  // current solution 'se', according 'gev' RETURNS: std::pair< uptr<Move<XES,
  // XEv, XSH>>, op<XEv> > note that this method is *not stateless* regarding
  // NSSeq class, as a *stateful* iterator variable is locally stored
  virtual std::pair<Move<XES, XSH>*, op<XEv>> findNext(
      GeneralEvaluator<XES, XSH>& gev, XES& se) {
    // checks if iterator is initialized or finished
    if (!it || it->isDone()) return std::make_pair(nullptr, std::nullopt);
    // finds next valid move
    it->nextValid(se);
    // gets current move (shall we test for isDone()?)
    uptr<Move<XES, XSH>> pm = it->current();
    // stores temporary raw pointer
    Move<XES, XSH>* m = pm.get();
    // gets cost for current move
    op<XEv> mvcost = gev.moveCost(*m, se);
    // if no cost, finishes
    if (!mvcost) return std::make_pair(nullptr, std::nullopt);
    // loops until finds improving move
    while (!gev.isStrictImprovement(*mvcost)) {
      // if done, finishes
      if (it->isDone()) return std::make_pair(nullptr, std::nullopt);
      it->nextValid(se);
      // gets current move (shall we test for isDone()?)
      pm = it->current();
      // stores temporary raw pointer
      m = pm.get();
      // gets cost for current move
      mvcost = gev.moveCost(*m, se);
      // if no cost, finishes
      if (!mvcost) return std::make_pair(nullptr, std::nullopt);
    }
    // current move should be improving
    return std::make_pair(pm.release(), mvcost);
  }

  // findBest (from NSFind): returns move that greatly improves current solution
  // 'se', according 'gev' NSFind is useful for exponential-sized neighborhoods,
  // without requiring any iterator structure
  virtual std::pair<Move<XES, XSH>*, op<XEv>> findBest(
      GeneralEvaluator<XES, XSH>& gev, XES& se) override {
    // finds first improving move
    std::pair<Move<XES, XSH>*, op<XEv>> mve = this->findFirst(gev, se);
    if (!mve.second) return std::make_pair(nullptr, std::nullopt);
    op<XEv> bestCost = std::move(mve.second);
    Move<XES, XSH>* bestMove = mve.first;
    // iterates while iterator is valid
    while (!it->isDone()) {
      // gets next improving move
      std::pair<Move<XES, XSH>*, op<XEv>> mveNext = this->findNext(gev, se);
      // checks if it surpasses existing best move
      if (mve.second && gev.betterStrict(*mve.second, *bestCost)) {
        bestCost = std::move(mve.second);
        bestMove = mve.first;
      }
    }
    return std::make_pair(bestMove, bestCost);
  }

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << NSFind<XES, XSH>::idComponent() << ":NSSeq";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NSFind<XES, XSH>::compatible(s));
  }
};

}  // namespace optframe

// General test for NSSeq
// TODO: only if not #include "printable.h"

// SHOULD NOT DO CTESTS HERE!
/*
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS

#include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>

namespace optframe {

using nsseq_test_base =
    NSSeq<pair<IsSolution<vector<int>, short>, Evaluation<>>>;

}  // namespace optframe
#endif  // cpp_concepts

*/

#endif /*OPTFRAME_NSSEQ_HPP_*/
