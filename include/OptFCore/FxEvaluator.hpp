// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXEVALUATOR_HPP_
#define OPTFCORE_FXEVALUATOR_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/Evaluator.hpp>

namespace optframe {

template <
    XESolution XES,
    bool Minimizing,
    typename XES::second_type (*fEvaluate)(const typename XES::first_type&)>
class FxEvaluator final : public Evaluator<typename XES::first_type,
                                           typename XES::second_type, XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
  XEv evaluate(const S& s) override {
    return fEvaluate(s);
  }

  bool isMinimization() const override {
    return Minimizing;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxEvaluator";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FXEVALUATOR_HPP_
