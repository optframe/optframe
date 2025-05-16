// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#pragma once

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

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

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XESolution XES2,
                     X2ESolution<XES2> X2ES = MultiESolution<XES2>>
#else
MOD_EXPORT template <typename XES, typename XES2,
                     typename X2ES = MultiESolution<XES2>>
#endif
class BasicSimulatedAnnealingBuilder : public GlobalSearchBuilder<XES>,
                                       public SA {
  // using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
  // using XM = Component; // only general builds here
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~BasicSimulatedAnnealingBuilder() {}

  // has sptr instead of sref, is that on purpose or legacy class?
  GlobalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                           std::string family = "") override {
    using modlog::LogLevel::Debug;
    using modlog::LogLevel::Warning;
    Log(Debug, &hf) << "BasicSA Builder" << std::endl;
    auto eval = hf.template tryAssign<GeneralEvaluator<XES>>(scanner);
    if (!eval.get()) {
      Log(Warning, &hf) << "BasicSA Builder failed" << std::endl;
      return nullptr;
    }
    sptr<GeneralEvaluator<XES>> ge{eval};

    int counter = 0;
    auto constructive = hf.template tryAssignIf<InitialSearch<XES>>(
        eval.get(), scanner, counter);
    auto _hlist = hf.template tryAssignListIf<NS<XES, XSH>>(constructive.get(),
                                                            scanner, counter);
    auto alpha = hf.tryAssignDoubleIf((bool)_hlist, scanner, counter);
    auto SAmax = hf.tryAssignDoubleIf((bool)alpha, scanner, counter);
    auto Ti = hf.tryAssignDoubleIf((bool)SAmax, scanner, counter);

    if (!Ti) {
      Log(Warning, &hf) << "BasicSA Builder failed" << std::endl;
      return nullptr;
    }

    vsref<NS<XES, XSH>> hlist;
    for (sptr<NS<XES, XSH>> x : *_hlist) hlist.push_back(x);

    if (Component::debug) {
      std::cout << "BasicSA Builder: got all parameters!" << std::endl;
      std::cout << "BasicSimulatedAnnealing with:" << std::endl;
      std::cout << "\teval=" << ge->id() << std::endl;
      std::cout << "\tconstructive=" << constructive->id() << std::endl;
      std::cout << "\t|hlist|=" << hlist.size() << std::endl;
      std::cout << "\thlist[0]=" << hlist[0]->id() << std::endl;
      std::cout << "\talpha=" << *alpha << std::endl;
      std::cout << "\tSAmax=" << *SAmax << std::endl;
      std::cout << "\tTi=" << *Ti << std::endl;
    }

    return new BasicSimulatedAnnealing<XES>(ge, constructive, hlist, *alpha,
                                            *SAmax, *Ti, hf.getRandGen());
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    // params.push_back(std::make_pair(GeneralEvaluator<XES>::idComponent(),
    // "evaluation function"));
    params.push_back(
        std::make_pair(Evaluator<typename XES::first_type,
                                 typename XES::second_type, XES>::idComponent(),
                       "evaluation function"));
    //
    // params.push_back(std::make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(std::make_pair(InitialSearch<XES>::idComponent(),
                                    "constructive heuristic"));
    std::stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of NS"));
    params.push_back(std::make_pair("OptFrame:double", "cooling factor"));
    params.push_back(std::make_pair(
        "OptFrame:int", "number of iterations for each temperature"));
    params.push_back(std::make_pair("OptFrame:double", "initial temperature"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicSimulatedAnnealing<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearchBuilder<XES>::idComponent() << SA::family() << "BasicSA";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe
