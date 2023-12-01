// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_

#include <string>
#include <utility>
#include <vector>
//
#include "../../LocalSearch.hpp"
#include "../../NS.hpp"
#include "../../Timer.hpp"

namespace optframe {

template <XESolution XES>
class RandomDescentMethod : public LocalSearch<XES> {
 private:
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)
  sref<GeneralEvaluator<XES>> evaluator;
  sref<NS<XES>> ns;
  unsigned int iterMax;

 public:
  RandomDescentMethod(sref<GeneralEvaluator<XES>> _eval, sref<NS<XES, XSH>> _ns,
                      unsigned int _iterMax)
      : evaluator(_eval), ns(_ns), iterMax(_iterMax) {}

  virtual ~RandomDescentMethod() = default;

  SearchStatus searchFrom(XES& se,
                          const StopCriteria<XEv>& stopCriteria) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;
    // Timer tNow;

    unsigned int iter = 0;

    // TODO: de-referentiation of 'target_f' WILL crash, if not provided!!
    // removing 'target_f'
    while ((iter < iterMax) && !stopCriteria.shouldStop(se.second)) {
      // uptr<Move<XES, XSH>> move = ns.randomMove(s);
      uptr<Move<XES, XSH>> move = ns->randomMove(se);

      op<XEv> cost = nullopt;

      // if (move && move->canBeApplied(s))
      if (move && move->canBeApplied(se)) {
        cost = evaluator->moveCost(*move, se);
      } else {
        iter++;
        continue;
      }

      iter++;

      // if (cost && evaluator.isImprovement(*cost))
      // if (cost && cost->isImprovingStrict())
      if (cost && evaluator->isStrictImprovement(*cost)) {
        move->applyUpdate(se);
        evaluator->reevaluate(se);
        iter = 0;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":RDM";
    return ss.str();
  }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class RandomDescentMethodBuilder : public LocalSearchBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~RandomDescentMethodBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<NS<XES, XSH>> ns;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(ns, id2, comp_id2);

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new RandomDescentMethod<XES>(eval, ns, iterMax);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NS<XES, XSH>::idComponent(), "neighborhood structure"));
    params.push_back(make_pair("OptFrame:int",
                               "max number of iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandomDescentMethod<XES>::idComponent();
  }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":RDM";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_
