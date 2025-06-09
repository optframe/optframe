// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

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

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BuilderILSL : public FamilyILS, public SingleObjSearchBuilder<XES> {
 public:
  ~BuilderILSL() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              std::string family = "") override {
    using modlog::LogLevel::Debug;
    using modlog::LogLevel::Warning;
    Log(Debug, &hf) << "ILSL Builder" << std::endl;
    int counter = 0;
    auto eval = hf.template tryAssign<GeneralEvaluator<XES>>(scanner);
    auto constructive = hf.template tryAssignIf<InitialSearch<XES>>(
        eval.get(), scanner, counter);

    if (!constructive) {
      Log(Warning, &hf) << "ILSL Builder failed" << std::endl;
      return nullptr;
    }
    // reload scanner for newer local search
    auto method = hf.createLocalSearch(scanner.rest());
    auto h = method.first;
    scanner = Scanner{method.second};
    //
    auto pert = hf.template tryAssignIf<ILSLPerturbation<XES>>((bool)h, scanner,
                                                               counter);
    auto iterMax = hf.tryAssignIntIf((bool)pert, scanner, counter);
    auto levelMax = hf.tryAssignIntIf((bool)iterMax, scanner, counter);

    if (!levelMax) {
      Log(Warning, &hf) << "ILSL Builder failed" << std::endl;
      return nullptr;
    }

    return new ILSL<XES>(eval, constructive, h, pert, *iterMax, *levelMax);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(std::make_pair(GeneralEvaluator<XES>::idComponent(),
                                    "evaluation function"));
    // params.push_back(std::make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(std::make_pair(InitialSearch<XES>::idComponent(),
                                    "constructive heuristic"));
    params.push_back(
        std::make_pair(LocalSearch<XES>::idComponent(), "local search"));
    params.push_back(std::make_pair(ILSLPerturbation<XES>::idComponent(),
                                    "ilsL perturbation"));
    params.push_back(
        std::make_pair("int", "max number of iterations without improvement"));
    params.push_back(std::make_pair("int", "levelMax of perturbation"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == ILSL<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << ":"
       << FamilyILS::family() << "ILSLevels";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe