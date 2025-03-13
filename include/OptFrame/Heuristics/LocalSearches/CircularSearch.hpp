// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/LocalSearch.hpp>

namespace optframe {

template <XESolution XES>
class CircularSearch : public LocalSearch<XES> {
  using XSH = XES;  // primary-based search type only (BestType)
  using XEv = typename XES::second_type;

 private:
  sref<GeneralEvaluator<XES>> eval;
  sref<NSEnum<XES, XSH>> ns;

  int initial_w;

 public:
  CircularSearch(sref<GeneralEvaluator<XES>> _eval,
                 sref<NSEnum<XES, XSH>> _nsEnum)
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
      uptr<Move<XES, XSH>> m = ns->indexMove(w);

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
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":CS";
    return ss.str();
  }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class CircularSearchBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~CircularSearchBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<NSEnum<XES, XES>> nsenum;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(nsenum, id2, comp_id2);

    // NOLINTNEXTLINE
    return new CircularSearch<XES>(eval, nsenum);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSEnum<XES, XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == CircularSearch<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":CS";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
