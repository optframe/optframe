// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_
#define OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Helper/Solution.hpp>

#include "GRASPFamily.h"

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

// Greedy Randomized Constructive
MOD_EXPORT template <XSolution S>
class GRConstructive : public Constructive<S>, public GRASP {
 public:
  virtual ~GRConstructive() {}

  virtual std::optional<S> generateGRSolution(float alpha,
                                              double timelimit) = 0;

  virtual std::optional<S> generateSolution(double timelimit) override {
    return generateGRSolution(1.0, timelimit);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << GRASP::family()
       << ":GRConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// BasicGRConstructive can envelop a Constructive
template <XSolution S>
class BasicGRConstructive : public GRConstructive<S> {
 public:
  Constructive<S>& c;

  BasicGRConstructive(Constructive<S>& _c) : c(_c) {}

  virtual ~BasicGRConstructive() {}

  virtual op<S> generateGRSolution(float alpha, double timelimit) override {
    // ignoring alpha
    return c.generateSolution(timelimit);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << GRASP::family()
       << ":GRConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif /*OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_*/
