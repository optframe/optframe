// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FNSENUM_HPP_
#define OPTFCORE_FNSENUM_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/NSEnum.hpp>

// no need for coroutines

namespace optframe {

template <XESolution XES>
class FNSEnum final : public NSEnum<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  std::function<unsigned int()> fSize;
  std::function<uptr<Move<XES>>(unsigned int k)> fIndex;
  // random generator
  sref<RandGen> rg;
  //
 public:
  FNSEnum(std::function<unsigned int()> fSize,
          std::function<uptr<Move<XES>>(unsigned int k)> fIndex,
          sref<RandGen> _rg)
      : NSEnum<XES>(_rg), fSize{fSize}, fIndex{fIndex}, rg{_rg} {}

  uptr<Move<XES, XSH>> indexMove(unsigned int index) override {
    return fIndex(index);
  }

  unsigned int size() const override { return fSize(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NSEnum<XES>::idComponent() << ":FxNSEnum";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FNSENUM_HPP_
