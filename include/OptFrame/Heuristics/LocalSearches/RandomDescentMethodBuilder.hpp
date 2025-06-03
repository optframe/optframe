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

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class RandomDescentMethodBuilder : public LocalSearchBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~RandomDescentMethodBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          std::string family = "") override {
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

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NS<XES, XSH>::idComponent(), "neighborhood structure"));
    params.push_back(std::make_pair(
        "OptFrame:int", "max number of iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandomDescentMethod<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":RDM";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe