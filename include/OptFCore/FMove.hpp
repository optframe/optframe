// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FMOVE_HPP_
#define OPTFCORE_FMOVE_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/Move.hpp>

namespace optframe {

template <class M, XESolution XES>
auto fDefaultCanBeApplied =
    [](const M&, const XES&) -> bool {
  return true;
};
//
template <class M, XESolution XES>
auto fDefaultCompareEq =
    [](const M& me, const Move<XES>& other) -> bool {
  return false;
};
//
template <class M, XESolution XES>
class FMove final : public Move<XES, typename XES::second_type> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective
  using Self = FMove<M, XES>;

 public:
  M m;  // internal structure for move

#ifdef OPTFCORE_FUNC_STATIC
#define OPTFCORE_FUNC_STATIC_FMOVE
#endif

  // force static on move, if necessary
  // #define OPTFCORE_FUNC_STATIC_FMOVE

#ifdef OPTFCORE_FUNC_STATIC_FMOVE
  typedef M (*FuncTypeMoveApply)(const M&, XES&);
  typedef bool (*FuncTypeMoveCBA)(const M&, XES&);
  typedef bool (*FuncTypeMoveEq)(const M&, const Move<XES>&);
#else
  typedef std::function<M(const M&, XES&)> FuncTypeMoveApply;
  typedef std::function<bool(const M&, const XES&)> FuncTypeMoveCBA;
  typedef std::function<bool(const M&, const Move<XES>&)> FuncTypeMoveEq;
#endif

  FuncTypeMoveApply fApply;
  FuncTypeMoveCBA fCanBeApplied = fDefaultCanBeApplied<M, XES>;
  FuncTypeMoveEq fCompareEq = fDefaultCompareEq<M, XES>;

  FMove(const M& _m,
        FuncTypeMoveApply _fApply,
        FuncTypeMoveCBA _fCanBeApplied = fDefaultCanBeApplied<M, XES>,
        FuncTypeMoveEq _fCompareEq = fDefaultCompareEq<M, XES>)
      : m{_m},
        fApply{_fApply},
        fCanBeApplied{_fCanBeApplied},
        fCompareEq{_fCompareEq} {
  }

  bool canBeApplied(const XES& se) override {
    return fCanBeApplied(m, se);
  }

  // NOTE: fApply will require a reverse move to ALWAYS exist.
  // FCore must be simple! Otherwise, just use fallback class-based mode
  uptr<Move<XES, XEv, XSH>> apply(XSH& se) override {
    return uptr<Move<XES, XEv, XSH>>{
        new Self{fApply(m, se), fApply, fCanBeApplied, fCompareEq}};
  }

  bool operator==(const Move<XES, XEv, XSH>& move) const override {
    const Move<XES>& move2 = (Move<XES>&)move;
    return fCompareEq(this->m, move2);
  }

  bool operator!=(const Move<XES, XEv, XSH>& m) const {
    return !(*this == m);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES, typename XES::second_type>::idComponent() << ":FMove";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  // use 'operator<<' for M
  void print() const override {
    std::cout << m << std::endl;
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FMOVE_HPP_
