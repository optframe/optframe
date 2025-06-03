// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
//

#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
#include <OptFrame/Search/InitialPopulation.hpp>

#include "ConstructiveRK.hpp"

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
MOD_EXPORT template <XESolution XES, class KeyType = double,
                     X2ESolution<XES> X2ES = VEPopulation<XES>>
#else
MOD_EXPORT template <typename XES, class KeyType = double,
                     typename X2ES = VEPopulation<XES>>
#endif
class InitialEPopulationRK : public InitialEPopulation<XES, X2ES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~InitialEPopulationRK() = default;

  // By default, this generator will evaluate ESolution pairs here,
  // but user can opt-out of this, leaving evaluations for the future
  // See BRKGA on a implementation over this concept.
  bool canEvaluate() const override { return true; }

  X2ES generateEPopulation(unsigned populationSize,
                           double timelimit) override = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (InitialEPopulation<XES, X2ES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialEPopulation<XES, X2ES>::idComponent() << ":" << EA::family()
       << ":" << RK::family() << "InitialEPopulationRK";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_
