// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
#define OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// C++
#include <string>
#include <utility>
//
#include <OptFrame/Concepts/Domain.hpp>
#include <OptFrame/Core/Evaluation.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XSolution S>
class GeneralCrossover : public Component {
 public:
  virtual ~GeneralCrossover() {}

  virtual std::pair<std::optional<S>, std::optional<S>> cross(const S&,
                                                              const S&) = 0;

 public:
  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GeneralCrossover"
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
       << Domain::getAlternativeDomain<S>("<XS>");
#else
       << Domain::getAlternativeDomain<S>("<X?>");
#endif
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
