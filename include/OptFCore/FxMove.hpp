// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXMOVE_HPP_
#define OPTFCORE_FXMOVE_HPP_

#include <functional>
#include <string>
#include <utility>
//
#include <OptFrame/Move.hpp>

namespace optframe {

template <class M, XESolution XES>
auto fxDefaultCanBeApplied = [](const M&, const XES&) -> bool { return true; };
//
template <class M, XESolution XES>
auto fxDefaultCompareEq =
    [](const M& me, const Move<XES>& other) -> bool { return false; };
//
template <
    class M, XESolution XES, M (*fApply)(const M&, XES&),
    bool (*fCanBeApplied)(const M&, const XES&) = fxDefaultCanBeApplied<M, XES>,
    bool (*fCompareEq)(const M&, const Move<XES>&) = fxDefaultCompareEq<M, XES>>
class FxMove final : public Move<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective
  using Self = FxMove<M, XES, fApply, fCanBeApplied, fCompareEq>;

 public:
  M m;  // internal structure for move

  // template static functions
  static M fs_apply(const M& m, XES& xes) { return fApply(m, xes); }

  FxMove(M&& _m) noexcept : m(std::move(_m)) {
    // std::cout << "FMove() => " << m << " address=" << this << std::endl;
  }

  FxMove(const M& _m) noexcept : m(_m) {
    // std::cout << "FMove() => " << m << " address=" << this << std::endl;
  }

  bool canBeApplied(const XES& se) override { return fCanBeApplied(m, se); }

  uptr<Move<XES, XSH>> apply(XSH& se) override {
    // fApply will require a reverse move to ALWAYS exist.
    // FCore must be simple! Otherwise, just use fallback class-based mode
    return uptr<Move<XES, XSH>>{new Self{fApply(m, se)}};
  }

  bool operator==(const Move<XES, XSH>& move) const override {
    const Move<XES>& move2 = (Move<XES>&)move;
    return fCompareEq(this->m, move2);
  }

  bool operator!=(const Move<XES, XSH>& m) const { return !(*this == m); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxMove";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  // use 'operator<<' for M
  void print() const override {
    // TODO(igormcoelho): should we require this printability on M type?
    std::cout << m << std::endl;
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FXMOVE_HPP_
