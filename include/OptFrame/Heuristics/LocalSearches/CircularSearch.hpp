// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_

#include <string>
#include <utility>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class CircularSearch : public LocalSearch<XES, XEv> {
 private:
  sref<GeneralEvaluator<XES, XEv>> eval;
  sref<NSEnum<XES, XEv>> ns;

  int initial_w;

 public:
  CircularSearch(sref<GeneralEvaluator<XES, XEv>> _eval,
                 sref<NSEnum<XES, XEv>> _nsEnum)
      : eval(_eval), ns(_nsEnum) {
    initial_w = 0;
  }

  virtual ~CircularSearch() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& sosc) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;

    // double timelimit = sosc.timelimit;
    // double target_f = sosc.target_f;
    int Wmax = ns->size();

    int w = initial_w % Wmax;

    do {
      uptr<Move<XES, XEv>> m = ns->indexMove(w);

      // if (m->canBeApplied(s)) {
      if (m->canBeApplied(se)) {
        bool mayEstimate = false;
        /// MoveCost<>& cost = *eval->moveCost(m, se, mayEstimate);
        op<XEv> cost = eval->moveCost(*m, se, mayEstimate);

        // if (eval->isImprovement(*cost)) {
        // if (cost->isImprovingStrict()) {
        if (eval->isStrictImprovement(*cost)) {
          // double old_f = e.evaluation();

          m->applyUpdate(se);
          eval->reevaluate(se);  // updates 'e'

          // cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << "
          // (antiga fo="<< old_f << ")" << endl << endl;

          initial_w = w + 1;

          return SearchStatus::IMPROVEMENT;
        }
      }

      w = (w + 1) % Wmax;
    } while (w != initial_w);
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << ":CS";
    return ss.str();
  }

  string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class CircularSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~CircularSearchBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<NSEnum<XES, XEv>> nsenum;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(nsenum, id2, comp_id2);

    // NOLINTNEXTLINE
    return new CircularSearch<XES, XEv>(eval, nsenum);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(NSEnum<XES, XEv>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == CircularSearch<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":CS";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
