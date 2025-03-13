// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_
#define OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_

// C++
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/Domain.hpp>
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Helper/MultiESolution.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/MultiEvaluator.hpp>

// #include "EA.hpp"

// using namespace std;

namespace optframe {

// IMPORTANT: by default, MultiESolution is generating VEPopulation types
// Because MultiESolution class is currently broken... Too bad!

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, X2ESolution<XES> X2ES = VEPopulation<XES>>
#else
template <XESolution XES, typename X2ES = VEPopulation<XES>>
#endif
class InitialMultiESolution : public Component {
 public:
  ~InitialMultiESolution() override = default;

  virtual X2ES generateEPopulation(unsigned populationSize) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":InitialMultiESolution"
       << Domain::getAlternativeDomain<X2ES>("<X2ESf64>");
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// ================================================================
// BasicInitialMultiESolution<XES> -> generates MultiESolution<XES>
// ================================================================
// IMPORTANT: by default, MultiESolution is generating VEPopulation types
// Because MultiESolution class is currently broken... Too bad!
// ================================================================

// TODO(igormcoelho): should we allow this to initialize multiple
//  "MultiESolution"-like types?
// I mean, this could initialize VEPopulation, EPopulation, etc...
// Maybe this is useful! This can be done!
//  But this overcomplicates things with builders...
//  so, we need some specific type anyway.
//
template <XESolution XES>
class BasicInitialMultiESolution : public InitialMultiESolution<XES> {
  using XEv = typename XES::second_type;

 public:
  using S = typename XES::first_type;

  sref<Constructive<S>> constructive;
  sref<IEvaluator<XES>> evaluator;

  explicit BasicInitialMultiESolution(sref<Constructive<S>> _constructive,
                                      sref<IEvaluator<XES>> _evaluator)
      : constructive{_constructive}, evaluator{_evaluator} {}

  virtual ~BasicInitialMultiESolution() {}

  VEPopulation<XES> generateEPopulation(unsigned populationSize) override {
    VEPopulation<XES> p;
    // TODO(igormcoelho): generatePopulation must have timelimit!
    double timelimit = 0.0;
    for (unsigned i = 0; i < populationSize; i++) {
      S s = *constructive->generateSolution(timelimit);
      XEv e = evaluator->evaluate(s);
      p.push_back({s, e});
    }
    return p;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (InitialMultiESolution<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialMultiESolution<XES>::idComponent();
    ss << ":BasicInitialMultiESolution";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// MultiESolution = VEPopulation
// This has nothing to do with MultiObjective stuff...

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>  // XEMSolution XMES
#else
template <typename XES>  // TODO: make MultiBuilder for XMES ?
#endif
class BasicInitialMultiESolutionBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~BasicInitialMultiESolutionBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    //
    sptr<Constructive<S>> c;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(c, id_0, sid_0);

    // Reading concrete MultiEvaluator as an IEvaluator
    // TODO: we NEED multiple buildComponent... implement that ASAP!
    //
    // This is IEvaluator now... another builder could be focused in XMES
    // types...
    //
    sptr<Evaluator<S, XEv, XES>> _ev;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(_ev, id_1, sid_1);
    sptr<IEvaluator<XES>> _iev = _ev;
    sref<IEvaluator<XES>> ev{_iev};

    return new BasicInitialMultiESolution<XES>(c, ev);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Constructive<S>::idComponent(), "constructive"));
    params.push_back(
        make_pair(Evaluator<S, XEv, XES>::idComponent(), "evaluator"));

    return params;
  }

  bool canBuild(string component) override {
    return component == BasicInitialMultiESolution<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent();
    ss << "BasicInitialMultiESolution";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// ===================================================================
// MultiESolution = VEPopulation
// This has nothing to do with MultiObjective stuff...
// However, this is a ComponentMultiBuilder, so it's intended for XMES.
// ===================================================================

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XEMSolution XMES>
#else
template <typename XES, typename XMES>
#endif
class BasicInitialMultiESolutionMultiBuilder
    : public ComponentMultiBuilder<XMES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  // using XMEv = MultiEvaluation<typename XEv::objType>;

 public:
  virtual ~BasicInitialMultiESolutionMultiBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    //
    sptr<Constructive<S>> c;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(c, id_0, sid_0);

    // Reading concrete MultiEvaluator as an IEvaluator
    // TODO: we NEED multiple buildComponent... implement that ASAP!
    //
    // This is IEvaluator now... another builder could be focused in XMES
    // types...
    //
    sptr<MultiEvaluator<XES, XMES>> _mev;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(_mev, id_1, sid_1);
    sptr<IEvaluator<XES>> _imev = _mev;
    sref<IEvaluator<XES>> mev{_imev};

    return new BasicInitialMultiESolution<XMES>(c, mev);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Constructive<S>::idComponent(), "constructive"));
    params.push_back(
        make_pair(MultiEvaluator<XES, XMES>::idComponent(), "multi evaluator"));

    return params;
  }

  bool canBuild(string component) override {
    return component == BasicInitialMultiESolution<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent();
    ss << "BasicInitialMultiESolution";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_
