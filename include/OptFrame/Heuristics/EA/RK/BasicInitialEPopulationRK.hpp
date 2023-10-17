// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_

#include <string>
#include <utility>
#include <vector>
//

#include "ConstructiveRK.hpp"
#include "InitialEPopulationRK.hpp"

namespace optframe {

template <XESolution XES, class KeyType = double,
          X2ESolution<XES> X2ES = VEPopulation<XES>>
class BasicInitialEPopulationRK
    : public InitialEPopulationRK<XES, KeyType, X2ES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  //
  // TODO: static_assert that S = std::vector<KeyType>
  using RealS = std::vector<KeyType>;

  static_assert(std::is_same_v<S, RealS>);

 public:
  sref<ConstructiveRK<KeyType>> constructiveRK;

  explicit BasicInitialEPopulationRK(
      sref<ConstructiveRK<KeyType>> _constructiveRK)
      : constructiveRK{_constructiveRK} {}

  // NOLINTNEXTLINE
  virtual ~BasicInitialEPopulationRK() override = default;

  // cannot evaluate here (return 'false')
  bool canEvaluate() const override { return false; }

  X2ES generateEPopulation(unsigned populationSize, double timelimit) override {
    X2ES p;
    for (unsigned i = 0; i < populationSize; i++) {
      auto ops = constructiveRK->generateSolution(timelimit);
      // cannot accept nullopt solutions at this point...
      assert((bool)ops);
      p.push_back(XES{*ops, XEv{}});
    }
    return p;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (InitialEPopulationRK<XES, KeyType, X2ES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialEPopulationRK<XES, KeyType, X2ES>::idComponent()
       << ":BasicInitialEPopulationRK";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class BasicInitialEPopulationRKBuilder
    : public ComponentBuilder<S, XEv, XES, X2ES> {
  using KeyType = double;
  using RealS = std::vector<KeyType>;
  using RealXEv = Evaluation<>;
  using RealXES = std::pair<RealS, RealXEv>;

 public:
  virtual ~BasicInitialEPopulationRKBuilder() = default;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    sptr<ConstructiveRK<KeyType>> crk;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(crk, id_0, sid_0);

    return new BasicInitialEPopulationRK<RealXES, double>(crk);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(ConstructiveRK<KeyType>::idComponent(), "constructive_rk"));

    return params;
  }

  bool canBuild(string component) override {
    return component ==
           BasicInitialEPopulationRK<RealXES, double>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << EA::family()
       << ":" << RK::family() << "BasicInitialEPopulationRKBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_
