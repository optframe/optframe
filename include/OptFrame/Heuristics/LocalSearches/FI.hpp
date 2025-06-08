// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

// =========================================================================
// This is First Improvement (FI)
// A classic neighborhood exploration technique, typically used as part of
//   Hill Climbing to perform a quick descent (small steps)
// Reference paper: unknown (classic)
// =========================================================================

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

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
class FI : public LocalSearch<XES> {
  using XEv = typename XSH::second_type;

 private:
  sref<GeneralEvaluator<XES, XSH>> eval;
  sref<NSSeq<XES, XSH>> nsSeq;

 public:
  FI(sref<GeneralEvaluator<XES>> _eval, sref<NSSeq<XES, XSH>> _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {}

  ~FI() override = default;

  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stop) override {
    if (Component::verbose) {
      std::cout << "FI: searchFrom begins" << std::endl;
      std::cout << eval->id() << std::endl;
    }

    if (Component::verbose) std::cout << "FI: getIterator" << std::endl;
    uptr<NSIterator<XES>> it = nsSeq->getIterator(se);
    //
    if (!it) {
      if (Component::warning)
        std::cout << "FI(WARNING): getIterator FAILED!" << std::endl;
      return SearchStatus::FAILED;  // poor implementation
    }
    //
    std::string bestMoveId = "";
    if (Component::verbose) std::cout << "FI: it->first()" << std::endl;
    it->first();

    if (it->isDone()) {
      if (Component::warning)
        std::cout << "FI(WARN): empty neighborhood!" << std::endl;
      return SearchStatus::NO_REPORT;
    }

    do {
      if (Component::verbose) std::cout << "FI: it->current()" << std::endl;
      uptr<Move<XES, XSH>> move = it->current();

      if (!move) {
        if (Component::warning)
          std::cout << "FI(WARNING): it->current() FAILED!" << std::endl;
        return SearchStatus::FAILED;  // poor implementation
      }

      if (Component::verbose)
        std::cout << "FI: move->canBeApplied()" << std::endl;
      if (move->canBeApplied(se)) {
        if (this->acceptsImprove(*move, se)) {
          // has improvement: this is an IMPROVEMENT situation!
          return this->onFinishLocal(*this, SearchStatus::IMPROVEMENT, se,
                                     stop);
        }
      }

      if (Component::verbose) std::cout << "FI: it->next()" << std::endl;
      it->next();
    } while (!it->isDone());

    // no improvement: this is a LOCAL_OPT situation!
    return this->onFinishLocal(*this, SearchStatus::LOCAL_OPT, se, stop);
  }

  // used on FirstImprovement
  // Accept and apply move if it improves parameter moveCost
  bool acceptsImprove(Move<XES, XSH>& m, XSH& se, bool allowEstimated = false) {
    if (Component::verbose)
      std::cout << "FI: begin acceptsImprove()" << std::endl;
    XEv& e = se.second;

    // try to get a cost
    op<XEv> p = m.cost(se, allowEstimated);

    // if p not null => much faster (using cost)
    if (p) {
      // verify if m is an improving move
      if (eval->isStrictImprovement(*p)) {
        // apply move and get reverse
        uptr<Move<XES, XSH>> rev = m.apply(se);
        // update value using calculated cost
        p->update(e);
        return true;
      } else {
        return false;
      }
    } else {
      if (Component::verbose) std::cout << "FI: no cost" << std::endl;
      // need to update 's' together with reevaluation of 'e' => slower (may
      // perform reevaluation)

      // TODO: in the future, consider moves with nullptr reverse (must save
      // original solution/evaluation)
      assert(m.hasReverse());

      // saving previous evaluation
      XEv ev_begin(e);

      // apply move to both XEv and Solution
      uptr<Move<XES, XSH>> rev = eval->applyMoveReevaluate(m, se);

      // compute cost directly on Evaluation
      XEv mcost = ev_begin.diff(se.second);

      if (Component::verbose)
        std::cout << "FI: isStrictImprovement?" << std::endl;
      // check if it is improvement
      if (eval->isStrictImprovement(mcost)) {
        return true;
      }

      // must return to original situation

      if (Component::verbose)
        std::cout << "FI: No improvement. Will reverse." << std::endl;

      uptr<Move<XES, XSH>> ini = rev->apply(se);
      // for now, must be not nullptr
      assert(ini != nullptr);
      // TODO: include management for 'false' hasReverse()
      assert(rev->hasReverse() && ini);
      e = std::move(ev_begin);
      //==================================================================

      if (Component::verbose)
        std::cout << "FI: ends acceptsImprove()" << std::endl;

      return false;
    }
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":FI";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }

  std::string toString() const override {
    std::stringstream ss;
    ss << "FI: " << nsSeq->toString();
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_FI_HPP_*/
