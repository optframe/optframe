// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_

#include <string>
#include <utility>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class HillClimbing : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> evaluator;
  sref<LocalSearch<XES>> ls;

 public:
  HillClimbing(sref<GeneralEvaluator<XES>> _ev, sref<LocalSearch<XES>> _ls)
      : evaluator(_ev), ls(_ls) {}

  virtual ~HillClimbing() = default;

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& sosc) override {
    if (Component::information)
      std::cout << "HillClimbing (ls=" << ls->toString() << ") starts."
                << std::endl;
    // S& s = se.first;
    XEv& e = se.second;
    //
    // double timelimit = sosc.timelimit;
    // long tini = time(nullptr);

    // Evaluation<>* e0 = &e.clone();
    XEv e0(e);  // avoid that using return status on 'exec'

    if (Component::information)
      std::cout << "HillClimbing invoking ls=" << ls->toString() << std::endl;
    ls->searchFrom(se, sosc);

    // long tnow = time(nullptr);

    // while improvement is found
    // while ((evaluator.betterThan(e, e0)) && ((tnow - tini) < timelimit))
    // while ((e.betterStrict(e0)) && ((tnow - tini) < timelimit))
    while ((evaluator->betterStrict(e, e0)) &&
           !sosc.shouldStop(e)) {  // ((tnow - tini) < timelimit)) {
      // delete e0;
      // e0 = &e.clone();

      //   (*e0) = e;
      e0 = e;

      ls->searchFrom(se, sosc);

      // tnow = time(nullptr);
    }

    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":HC";
    return ss.str();
  }

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    // evaluator->setVerboseR();
    return ls->setMessageLevelR(ll);
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class HillClimbingBuilder : public LocalSearchBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~HillClimbingBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    // NOLINTNEXTLINE
    return new HillClimbing<XES>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == HillClimbing<XES, XEv>::idComponent();
  }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":HC";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_
