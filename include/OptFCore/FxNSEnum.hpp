// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXNSENUM_HPP_
#define OPTFCORE_FXNSENUM_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/NSEnum.hpp>

// no need for coroutines

namespace optframe {

template <XESolution XES,
          unsigned int (*fSize)(),
          uptr<Move<XES>> (*fIndex)(unsigned int k)>
class FxNSEnum final : public NSEnum<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  // random generator
  sref<RandGen> rg;
  //
 public:
  FxNSEnum(sref<RandGen> _rg)
      : NSEnum<XES>(_rg),
        rg{_rg} {
  }

  uptr<Move<XES, XEv>> indexMove(unsigned int index) override {
    return fIndex(index);
  }

  unsigned int size() const override {
    return fSize();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxNSEnum";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FXNSENUM_HPP_
