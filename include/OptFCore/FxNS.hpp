// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXNS_HPP_
#define OPTFCORE_FXNS_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/NS.hpp>

namespace optframe {

template <
    XESolution XES,
    uptr<Move<XES>> (*fRandom)(const XES&)>
class FxNS final : public NS<XES, typename XES::second_type> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
  uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override {
    return fRandom(se);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxNS";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FXNS_HPP_
