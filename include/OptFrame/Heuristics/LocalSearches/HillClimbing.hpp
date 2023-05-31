// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class HillClimbing : public LocalSearch<XES, XEv> {
 private:
  sref<GeneralEvaluator<XES, XEv>> evaluator;
  sref<LocalSearch<XES, XEv>> ls;

 public:
  HillClimbing(sref<GeneralEvaluator<XES, XEv>> _ev,
               sref<LocalSearch<XES, XEv>> _ls)
      : evaluator(_ev), ls(_ls) {}

  virtual ~HillClimbing() {}

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
           !sosc.shouldStop(e)) {  //((tnow - tini) < timelimit)) {
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
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << ":HC";
    return ss.str();
  }

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    // evaluator->setVerboseR();
    return ls->setMessageLevelR(ll);
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class HillClimbingBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~HillClimbingBuilder() {}

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES, XEv>> h = method.first;

    scanner = Scanner(method.second);

    return new HillClimbing<XES, XEv>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == HillClimbing<XES, XEv>::idComponent();
  }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":HC";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
