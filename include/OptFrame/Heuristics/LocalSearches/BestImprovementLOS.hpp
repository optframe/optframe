// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_BESTIMPROVEMENTLOS_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_BESTIMPROVEMENTLOS_HPP_

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Timer.hpp>
#include <string>
#include <utility>
#include <vector>

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class BestImprovementLOS : public LocalSearch<S, XEv, XSH> {
 private:
  Evaluator<XES, XEv>& eval;
  NSSeq<XES, XSH>& nsSeq;

 public:
  BestImprovementLOS(Evaluator<XES, XEv>& _eval, NSSeq<XES, XSH>& _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {}

  virtual ~BestImprovementLOS() {}

  virtual void exec(S& s, const StopCriteria<XEv>& sosc) override {
    Evaluation<> e = eval.evaluate(s);

    exec(s, e, sosc);
  }

  virtual void exec(S& s, Evaluation<>& e,
                    const StopCriteria<XEv>& sosc) override {
    Timer t;

    // TODO: use block iterator and manage each partial local optima discovered

    // NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(s);
    NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(se);

    std::cout << "TODO: BestImprovementLOS UNIMPLEMENTED!" << std::endl;

    return;

    // NSIterator<XES>& it = *nsSeq.getIterator(s);
    NSIterator<XES>& it = *nsSeq.getIterator(se);

    it.first();

    if (it.isDone()) {
      delete &it;
      return;
    }

    Move<S, XEv>* bestMove = it.current();

    /*if(e.getLocalOptimumStatus(bestMove->id()))
                {
                        delete &it;
                        delete bestMove;

                        sum_time += t.inMilliSecs();
                        return;
                }*/

    MoveCost<>* bestCost = nullptr;

    while (true) {
      // while (!bestMove->canBeApplied(s))
      while (!bestMove->canBeApplied(se)) {
        delete bestMove;
        it.next();
        if (!it.isDone()) {
          bestMove = it.current();
        } else {
          delete &it;
          return;
        }
      }

      bestCost = eval.moveCost(*bestMove, se);
      if (eval.isImprovement(*bestCost)) {
        it.next();
        break;
      } else {
        delete bestCost;
        delete bestMove;
        it.next();

        if (!it.isDone()) {
          bestMove = it.current();
        } else {
          delete &it;
          return;
        }
      }
    }

    // it.next();
    while (!it.isDone()) {
      Move<S, XEv>* move = it.current();
      if (move->canBeApplied(s)) {
        bool mayEstimate = false;
        MoveCost<>* cost = eval.moveCost(*move, se, mayEstimate);

        if (eval.betterThan(*cost, *bestCost)) {
          delete bestMove;
          delete bestCost;
          bestMove = move;
          bestCost = cost;
        } else {
          delete move;
          delete cost;
        }
      } else
        delete move;

      it.next();
    }

    if (eval.isImprovement(*bestCost)) {
      // std::cout << "MOVE IS IMPROVEMENT! cost=";
      // bestCost->print();

      if (bestCost->isEstimated()) {
        // TODO: have to test if bestMove is ACTUALLY an improvement move...
      }

      Component::safe_delete(bestMove->applyUpdate(e, s));

      eval.reevaluate(e, s);  // updates 'e'
                              // e.setLocalOptimumStatus(bestMove->id(), false);
                              // //set NS 'id' out of Local Optimum
    } else {
      // bestMove->updateNeighStatus(s.getADS());
      // e.setLocalOptimumStatus(bestMove->id(), true); //set NS 'id' on Local
      // Optimum
    }
    // std::cout << "#" << num_calls << " out_bi:";
    // bestMove->print();
    // nsSeq.print();
    // e.print();

    delete bestCost;
    delete bestMove;
    delete &it;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "BI_LOS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override { std::cout << toString() << std::endl; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "BI_LOS: " << nsSeq.toString();
    return ss.str();
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XESolution XES>
#else
template <typename XES>
#endif
class BestImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~BestImprovementBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          std::string family = "") override {
    if (!scanner.hasNext()) return nullptr;
    Evaluator<XES, XEv>* eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    if (!scanner.hasNext()) return nullptr;
    NSSeq<XES>* nsseq;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(nsseq, id2, comp_id2);

    // NOLINTNEXTLINE
    return new BestImprovementLOS<S, XEv>(*eval, *nsseq);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(std::make_pair(Evaluator<S, XEv, XES>::idComponent(),
                                    "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES, XSH>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BestImprovementLOS<S, XEv>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":BI_LOS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_BESTIMPROVEMENTLOS_HPP_
