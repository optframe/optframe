// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONSTRUCTIVE_RK_HPP_
#define OPTFRAME_CONSTRUCTIVE_RK_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <vector>
//
#include <OptFrame/Core/Constructive.hpp>

#include "../FamilyEA.hpp"
#include "FamilyRK.hpp"

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

MOD_EXPORT template <class KeyType>
class ConstructiveRK : public Constructive<std::vector<KeyType>> {
  using S = std::vector<KeyType>;

 public:
  virtual ~ConstructiveRK() = default;

  std::optional<S> generateSolution(double timelimit) override = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Constructive<S>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Constructive<S>::idComponent() << ":" << FamilyEA::family() << ":"
       << FamilyRK::family() << "ConstructiveRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};
//
}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_RK_HPP_*/
