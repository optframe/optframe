// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXCONSTRUCTIVE_HPP_
#define OPTFCORE_FXCONSTRUCTIVE_HPP_

#include <functional>
#include <optional>
#include <string>
//
#include <OptFrame/Core/Constructive.hpp>

namespace optframe {

template <XSolution S, std::optional<S> (*fGenerate)(double)>
class FxConstructive final : public Constructive<S> {
 public:
  std::optional<S> generateSolution(double timelimit) override {
    return fGenerate(timelimit);
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":FxConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FXCONSTRUCTIVE_HPP_
