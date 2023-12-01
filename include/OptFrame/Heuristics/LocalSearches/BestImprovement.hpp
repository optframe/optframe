// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Timer.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = typename XES::second_type,
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
    op<XEv> bestCost = nullopt;

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
      // cout << "MOVE IS IMPROVEMENT! cost=";
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
    // cout << "#" << num_calls << " out_bi:";
    // bestMove->print();
    // nsSeq.print();
    // e.print();
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":BI";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override { cout << toString() << endl; }

  virtual string toString() const override {
    stringstream ss;
    ss << "BI: " << nsSeq->toString();
    return ss.str();
  }

  virtual string log() const {
    stringstream ss;
    ss << sum_time;
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class BestImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~BestImprovementBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: BestImprovementBuilder::build()" << std::endl;
    if (!scanner.hasNext()) return nullptr;
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_ev_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int ev_id = *scanner.nextInt();
    hf.assign(eval, ev_id, comp_ev_id);

    if (!scanner.hasNext()) return nullptr;
    sptr<NSSeq<XES>> nsseq;
    std::string comp_nsseq_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int nsseq_id = *scanner.nextInt();
    hf.assign(nsseq, nsseq_id, comp_nsseq_id);

    return new BestImprovement<XES>(eval, nsseq);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(string component) override {
    return component == BestImprovement<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":BI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_BI_HPP_*/
