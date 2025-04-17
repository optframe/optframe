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
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BestImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~BestImprovementBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          std::string family = "") override {
    if (this->verbose)
      std::cout << "Debug: BestImprovementBuilder::build()" << std::endl;
    if (!scanner.hasNext()) return nullptr;
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_ev_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int ev_id = *scanner.nextInt();
    hf.assign(eval, ev_id, comp_ev_id);

    if (!scanner.hasNext()) return nullptr;
    sptr<NSSeq<XES>> nsseq;
    std::string comp_nsseq_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int nsseq_id = *scanner.nextInt();
    hf.assign(nsseq, nsseq_id, comp_nsseq_id);

    return new BestImprovement<XES>(eval, nsseq);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BestImprovement<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":BI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe