// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FDIRECTION_HPP_  // NOLINT
#define OPTFCORE_FDIRECTION_HPP_  // NOLINT

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <functional>
#include <string>
//
#include <OptFrame/Core/Direction.hpp>

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

MOD_EXPORT namespace optframe {
  enum MinOrMax { MINIMIZE, MAXIMIZE };

  template <XEvaluation XEv,     // E Type
            MinOrMax Minimizing  // is minimization
            >
  class FDirection final : public Direction<XEv> {
    using super = Direction<XEv>;

   public:
    bool isMinimization() const override {
      return Minimizing == MinOrMax::MINIMIZE;
    }

    bool compatible(std::string s) override {
      return (s == idComponent()) || (super::compatible(s));
    }

    static std::string idComponent() {
      std::stringstream ss;
      ss << super::idComponent() << ":FDirection";
      return ss.str();
    }

    std::string id() const override { return idComponent(); }
  };

}  // namespace optframe

#endif  // OPTFCORE_FDIRECTION_HPP_ // NOLINT
