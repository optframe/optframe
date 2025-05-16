// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#pragma once

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Heuristics/SA/SA.hpp>
#include <OptFrame/Hyper/WarmupBuilder.hpp>

#include "BasicSimulatedAnnealing.hpp"
#include "BasicSimulatedAnnealingBuilder.hpp"
#include "HelperSA.hpp"

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
class BasicSimulatedAnnealingWarmup : public WarmupBuilder<XES>, public SA {
  using VParameters = std::vector<std::pair<std::string, std::string>>;
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~BasicSimulatedAnnealingWarmup() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    // first warmup, then build with builder
    WarmupOutput wo = warmup(scanner, hf, family);
    if (!wo.config) {
      return nullptr;
    } else {
      Scanner scanner2{*wo.config};
      BasicSimulatedAnnealingBuilder<XES, XES2, X2ES> builder;
      return builder.build(scanner2, hf, family);
    }
  }

  WarmupOutput warmup(Scanner& scanner, HeuristicFactory<XES>& hf,
                      std::string family = "") override {
    using modlog::LogLevel::Debug;
    using modlog::LogLevel::Warning;
    Log(Debug, &hf) << "BasicSA Warmup" << std::endl;
    auto eval = hf.template tryAssign<GeneralEvaluator<XES>>(scanner);
    if (!eval.get()) {
      Log(Warning, &hf) << "BasicSA Warmup failed" << std::endl;
      return WarmupOutput{};
    }
    sptr<GeneralEvaluator<XES>> ge{eval};

    int counter = 0;
    sptr<InitialSearch<XES>> constructive =
        hf.tryAssignIf(eval.get(), scanner, counter);
    vsptr<NS<XES, XSH>> _hlist =
        hf.tryAssignListIf(constructive.get(), scanner, counter);
    op<double> alpha =
        hf.tryAssignDoubleIf(_hlist.size() > 0, scanner, counter);
    op<int> SAmax = hf.tryAssignDoubleIf(alpha, scanner, counter);
    op<double> Ti = hf.tryAssignDoubleIf(SAmax, scanner, counter);

    if (!Ti) {
      Log(Warning, &hf) << "BasicSA Warmup failed" << std::endl;
      return WarmupOutput{};
    }

    vsref<NS<XES, XSH>> hlist;
    for (sptr<NS<XES, XSH>> x : _hlist) hlist.push_back(x);

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

    return new BasicSimulatedAnnealing<XES>(ge, constructive, hlist, alpha,
                                            SAmax, Ti, hf.getRandGen());
  }

  VParameters parameters() override {
    VParameters params;
    params.push_back(
        std::make_pair(Evaluator<typename XES::first_type,
                                 typename XES::second_type, XES>::idComponent(),
                       "evaluation function"));
    params.push_back(std::make_pair(InitialSearch<XES>::idComponent(),
                                    "constructive heuristic"));
    std::stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of NS"));
    params.push_back(std::make_pair("OptFrame:double", "beta heating factor"));
    params.push_back(
        std::make_pair("OptFrame:double", "gama acceptance ratio"));
    params.push_back(std::make_pair(
        "OptFrame:int", "SAmax number of iterations for each temperature"));
    params.push_back(
        std::make_pair("OptFrame:double", "T0 initial cold temperature"));
    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicSimulatedAnnealing<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << WarmupBuilder<XES>::idComponent() << SA::family() << "BasicSA";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe
