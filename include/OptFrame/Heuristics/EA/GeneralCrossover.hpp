// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
#define OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_

// C++
#include <string>
#include <utility>
//
#include <OptFrame/Evaluation.hpp>
// #include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Domain.hpp>

namespace optframe {

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS>
template <XSolution S>
class GeneralCrossover : public Component {
 public:
  virtual ~GeneralCrossover() {
  }

  virtual pair<std::optional<S>, std::optional<S>>
  cross(const S&, const S&) = 0;

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

  std::string id() const override {
    return idComponent();
  }

  std::string toString() const override {
    return id();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
