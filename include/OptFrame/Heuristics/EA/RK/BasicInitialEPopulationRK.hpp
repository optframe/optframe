// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <string>
#include <utility>
#include <vector>
//

#include "ConstructiveRK.hpp"
#include "InitialEPopulationRK.hpp"

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
MOD_EXPORT template <XESolution XES, class KeyType = double,
                     X2ESolution<XES> X2ES = VEPopulation<XES>>
#else
MOD_EXPORT template <typename XES, class KeyType = double,
                     typename X2ES = VEPopulation<XES>>
#endif
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

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialEPopulationRK<XES, KeyType, X2ES>::idComponent()
       << ":BasicInitialEPopulationRK";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BasicInitialEPopulationRKBuilder : public ComponentBuilder<XES> {
  using KeyType = double;
  using RealS = std::vector<KeyType>;
  using RealXEv = Evaluation<>;
  using RealXES = std::pair<RealS, RealXEv>;

 public:
  virtual ~BasicInitialEPopulationRKBuilder() = default;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    sptr<ConstructiveRK<KeyType>> crk;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(crk, id_0, sid_0);

    return new BasicInitialEPopulationRK<RealXES, double>(crk);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(ConstructiveRK<KeyType>::idComponent(), "constructive_rk"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component ==
           BasicInitialEPopulationRK<RealXES, double>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << EA::family() << ":"
       << RK::family() << "BasicInitialEPopulationRKBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_BASICINITIALEPOPULATIONRK_HPP_
