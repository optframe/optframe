// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe


#ifndef OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_
//
#include <string>
#include <vector>
//
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

#include "DecoderRandomKeysNoEvaluation.hpp"
#include "OptFrame/Concepts/MyConcepts.hpp"

namespace optframe {

// this is an adapter based on Evaluator and DecoderRandomKeysNoEvaluation

template <XESolution XES, ConceptsComparability KeyType>
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

  virtual pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    if (!needsSolution) {
      if (Component::debug)
        std::cout << "DEBUG: cannot disable solution decoding!" << std::endl;
    }
    S s = decoderSol->decodeSolution(rk);
    XEv e = evaluator->evaluate(s);
    return pair<XEv, op<S>>{e, op<S>{s}};
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

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class BasicDecoderRandomKeysBuilder
    : public ComponentBuilder<S, XEv, XES, X2ES> {
  using KeyType = double;
  using RealS = std::vector<KeyType>;
  using RealXEv = Evaluation<>;
  using RealXES = std::pair<RealS, RealXEv>;

 public:
  virtual ~BasicDecoderRandomKeysBuilder() {}

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
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

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<S, XEv, XES>::idComponent(), "evaluator"));
    params.push_back(
        make_pair(DecoderRandomKeysNoEvaluation<S, KeyType>::idComponent(),
                  "decoder_no_eval"));

    return params;
  }

  bool canBuild(string component) override {
    return component == BasicDecoderRandomKeys<RealXES, double>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << EA::family()
       << ":" << RK::family() << "BasicDecoderRandomKeysBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_BASICDECODERRANDOMKEYS_HPP_
