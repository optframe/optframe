// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FDIRECTION_HPP_
#define OPTFCORE_FDIRECTION_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/Direction.hpp>

enum MinOrMax { MINIMIZE, MAXIMIZE };

namespace optframe {

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

#endif  // OPTFCORE_FDIRECTION_HPP_
