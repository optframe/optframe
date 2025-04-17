// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

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

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type,
                     XESolution XSH = XES>
class BestImprovement : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> eval;
  sref<NSSeq<XES, XSH>> nsSeq;

  // logs
  double sum_time;
  int num_calls;

 public:
  BestImprovement(sref<GeneralEvaluator<XES>> _eval,
                  sref<NSSeq<XES, XSH>> _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {
    sum_time = 0.0;
    num_calls = 0;
  }

  virtual ~BestImprovement() = default;

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& sosc) override
  //{
  //	Evaluation<> e = eval.evaluate(s);
  //	exec(s, e, sosc);
  //}

  virtual SearchStatus searchFrom(XSH& se,
                                  const StopCriteria<XEv>& sosc) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;

    // double timelimit = sosc.timelimit;
    // double target_f = sosc.target_f;

    if (Component::information)
      std::cout << "BI::starts for " << nsSeq->toString() << std::endl;

    num_calls++;
    Timer t;

    // TODO: verify if it's not null
    // uptr<NSIterator<XES, XSH>> it = nsSeq.getIterator(s);
    uptr<NSIterator<XES, XSH>> it = nsSeq->getIterator(se);
    //
    if (!it) return SearchStatus::FAILED;  // poor implementation
    //
    if (Component::information) std::cout << "BI::first()" << std::endl;
    //
    it->first();

    if (it->isDone()) {
      sum_time += t.inMilliSecs();
      return SearchStatus::NO_REPORT;  // Is it LOCAL_OPT?
    }

    uptr<Move<XES, XSH>> bestMove = it->current();

    /*if(e.getLocalOptimumStatus(bestMove->id()))
                {
                        delete &it;
                        delete bestMove;

                        sum_time += t.inMilliSecs();
                        return;
                }*/

    // MoveCost<>* bestCost = nullptr;
    op<XEv> bestCost = std::nullopt;

    while (true) {
      while (!bestMove->canBeApplied(se)) {
        it->next();
        if (!it->isDone()) {
          bestMove = it->current();
        } else {
          sum_time += t.inMilliSecs();
          return SearchStatus::NO_REPORT;  // Is it LOCAL_OPT ?
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
          return SearchStatus::NO_REPORT;  // Is it LOCAL_OPT ?
        }
      }
    }

    // it.next();
    while (!it->isDone()) {
      uptr<Move<XES, XSH>> move = it->current();
      // if (move->canBeApplied(s))
      if (move->canBeApplied(se)) {
        /// MoveCost<>* cost = eval.moveCost(*move, se);
        op<XEv> cost = eval->moveCost(*move, se);

        // if (eval.betterThan(*cost, *bestCost))
        // if (cost->betterStrict(*bestCost))
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

    if (eval->isStrictImprovement(*bestCost)) {
      // std::cout << "MOVE IS IMPROVEMENT! cost=";
      // bestCost->print();

      if (bestCost->isEstimated()) {
        // TODO: have to test if bestMove is ACTUALLY an improvement move...
      }

      bestMove->applyUpdate(se);

      eval->reevaluate(se);  // updates 'e'
                             // e.setLocalOptimumStatus(bestMove->id(), false);
                             // //set NS 'id' out of Local Optimum
      sum_time += t.inMilliSecs();
      return SearchStatus::IMPROVEMENT;
    } else {
      // bestMove->updateNeighStatus(s.getADS());
      // e.setLocalOptimumStatus(bestMove->id(), true); //set NS 'id' on Local
      // Optimum
      sum_time += t.inMilliSecs();
      return SearchStatus::LOCAL_OPT;
    }
    // std::cout << "#" << num_calls << " out_bi:";
    // bestMove->print();
    // nsSeq.print();
    // e.print();
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
