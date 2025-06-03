// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

#include "DecoderRandomKeysNoEvaluation.hpp"
#include "OptFrame/Concepts/MyConcepts.hpp"

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

// this is an adapter based on Evaluator and DecoderRandomKeysNoEvaluation

MOD_EXPORT template <XESolution XES, ConceptsComparability KeyType>
class BasicDecoderRandomKeys : public DecoderRandomKeys<XES, KeyType> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;
  using XES2 = std::pair<RSK, XEv>;
  using X2ES = VEPopulation<XES2>;

 public:
  sref<Evaluator<S, XEv, XES>> evaluator;
  sref<DecoderRandomKeysNoEvaluation<S, KeyType>> decoderSol;

  BasicDecoderRandomKeys(
      sref<Evaluator<S, XEv, XES>> _evaluator,
      sref<DecoderRandomKeysNoEvaluation<S, KeyType>> _decoderSol)
      : evaluator{_evaluator}, decoderSol{_decoderSol} {}

  virtual ~BasicDecoderRandomKeys() {}

  std::pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    if (!needsSolution) {
      if (Component::debug)
        std::cout << "DEBUG: cannot disable solution decoding!" << std::endl;
    }
    S s = decoderSol->decodeSolution(rk);
    XEv e = evaluator->evaluate(s);
    return std::pair<XEv, op<S>>{e, op<S>{s}};
  }

  bool isMinimization() const override { return evaluator->isMinimization(); }

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeys<XES, KeyType>::idComponent()
       << ":BasicDecoderRandomKeys";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XESolution XES>
#else
template <typename XES>
#endif
class BasicDecoderRandomKeysBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  //
  using KeyType = double;
  using RealS = std::vector<KeyType>;
  using RealXEv = Evaluation<>;
  using RealXES = std::pair<RealS, RealXEv>;

 public:
  virtual ~BasicDecoderRandomKeysBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    sptr<Evaluator<S, XEv, XES>> evaluator;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(evaluator, id_0, sid_0);

    sptr<DecoderRandomKeysNoEvaluation<S, KeyType>> decoderSol;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(decoderSol, id_1, sid_1);

    return new BasicDecoderRandomKeys<XES, double>(evaluator, decoderSol);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        std::make_pair(Evaluator<S, XEv, XES>::idComponent(), "evaluator"));
    params.push_back(
        std::make_pair(DecoderRandomKeysNoEvaluation<S, KeyType>::idComponent(),
                       "decoder_no_eval"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicDecoderRandomKeys<RealXES, double>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << EA::family() << ":"
       << RK::family() << "BasicDecoderRandomKeysBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_
