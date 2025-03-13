// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONSTRUCTIVE_RK_HPP_
#define OPTFRAME_CONSTRUCTIVE_RK_HPP_

// C++
#include <vector>
//
#include <OptFrame/Core/Constructive.hpp>

#include "../EA.hpp"
#include "RK.hpp"

namespace optframe {

template <class KeyType>
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
    ss << Constructive<S>::idComponent() << ":" << EA::family() << ":"
       << RK::family() << "ConstructiveRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};
//
}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_RK_HPP_*/
