// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

// =========================================================================
// This is Best Improvement (BI)
// A classic neighborhood exploration technique, typically used as part of
//   Hill Climbing to perform Steepest Descent.
// Reference paper: unknown (classic)
// =========================================================================

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Timer.hpp>

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

MOD_EXPORT template <XESolution XES, XESolution XSH = XES>
class BI : public LocalSearch<XES, XSH> {
  using XEv = typename XSH::second_type;

 private:
  sref<GeneralEvaluator<XES>> eval;
  sref<NSSeq<XES, XSH>> nsSeq;

 public:
  // internal logs
  double sum_time;
  int num_calls;

 public:
  BI(sref<GeneralEvaluator<XES>> _eval, sref<NSSeq<XES, XSH>> _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {
    sum_time = 0.0;
    num_calls = 0;
  }

  ~BI() override = default;

  SearchStatus searchFrom(XSH& se, const StopCriteria<XEv>& stop) override {
    if (Component::information)
      std::cout << "BI::starts for " << nsSeq->toString() << std::endl;

    num_calls++;
    Timer t;

    if (Component::debug) std::cout << "BI::getIterator()" << std::endl;
    uptr<NSIterator<XES, XSH>> it = nsSeq->getIterator(se);
    if (!it) return this->onFinishLocal(*this, SearchStatus::FAILED, se, stop);
    //
    if (Component::debug) std::cout << "BI::first()" << std::endl;
    it->first();

    if (it->isDone()) {
      sum_time += t.inMilliSecs();
      // Nothing to do? Empty iterator? Is it LOCAL_OPT? Or NO_REPORT?
      return this->onFinishLocal(*this, SearchStatus::LOCAL_OPT, se, stop);
    }

    uptr<Move<XES, XSH>> bestMove = it->current();
    op<XEv> bestCost = std::nullopt;

    while (true) {
      while (!bestMove->canBeApplied(se)) {
        it->next();
        if (!it->isDone()) {
          bestMove = it->current();
        } else {
          sum_time += t.inMilliSecs();
          // Nothing to do? Non-appliable moves? Is it LOCAL_OPT? Or NO_REPORT?
          // Maybe it's the end of search, with previously improving moves...
          // Maybe check if bestCost is std::nullopt here?
          return this->onFinishLocal(*this, SearchStatus::NO_REPORT, se, stop);
        }
      }

      bestCost = eval->moveCost(*bestMove, se);

      if (eval->isStrictImprovement(*bestCost)) {
        it->next();
        break;
      } else {
        it->next();
        if (!it->isDone()) {
          bestMove = it->current();
        } else {
          sum_time += t.inMilliSecs();
          // Nothing to do? No improvement move? Is it LOCAL_OPT? Or NO_REPORT?
          // Maybe check if bestCost is std::nullopt here?
          return this->onFinishLocal(*this, SearchStatus::NO_REPORT, se, stop);
        }
      }
    }

    while (!it->isDone()) {
      uptr<Move<XES, XSH>> move = it->current();
      if (move->canBeApplied(se)) {
        op<XEv> cost = eval->moveCost(*move, se);

        if (eval->betterStrict(*cost, *bestCost)) {
          bestMove = std::move(move);
          move = nullptr;
          bestCost = cost;  // TODO: std::move?
        } else {
          // ?
        }
      } else {
        // ?
      }

      it->next();
    }

    // ===============================================
    // finalize local search: IMPROVEMENT OR LOCAL_OPT
    // ===============================================

    if (eval->isStrictImprovement(*bestCost)) {
      if (bestCost->isEstimated()) {
        // TODO: have to test if bestMove is ACTUALLY an improvement move...
      }

      bestMove->applyUpdate(se);

      eval->reevaluate(se);

      sum_time += t.inMilliSecs();
      // has improvement: this is an IMPROVEMENT situation!
      return this->onFinishLocal(*this, SearchStatus::IMPROVEMENT, se, stop);
    } else {
      sum_time += t.inMilliSecs();
      // no improvement: this is a LOCAL_OPT situation!
      return this->onFinishLocal(*this, SearchStatus::LOCAL_OPT, se, stop);
    }
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":BI";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override { std::cout << toString() << std::endl; }

  virtual std::string toString() const override {
    std::stringstream ss;
    ss << "BI: " << nsSeq->toString();
    return ss.str();
  }

  virtual std::string log() const {
    std::stringstream ss;
    ss << sum_time;
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_BI_HPP_*/
