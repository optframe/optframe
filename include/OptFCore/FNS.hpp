// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FNS_HPP_
#define OPTFCORE_FNS_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/NS.hpp>

namespace optframe {

template <XESolution XES, typename ProblemType = void>
class FNS final : public NS<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(sref<ProblemType>, const XES&);
  typedef uptr<Move<XES>> (*FuncTypeNSValidRand)(sref<ProblemType>, const XES&);
#else
  typedef std::function<uptr<Move<XES>>(sref<ProblemType>, const XES&)>
      FuncTypeNSRand;
  typedef FuncTypeNSRand FuncTypeNSValidRand;  // same type
#endif

  sref<ProblemType> p;
  FuncTypeNSRand fRandom;

  // standard implementation for fValidRandom with ProblemType
  FuncTypeNSValidRand fValidRandom{
      [this](sref<ProblemType> _p, const XES& se) -> uptr<Move<XES, XEv, XSH>> {
        uptr<Move<XES, XEv, XSH>> moveValid = this->fRandom(_p, se);
        if (moveValid && moveValid->canBeApplied(se))
          return moveValid;
        else
          return nullptr;
      }};

  FNS(sref<ProblemType> _p, FuncTypeNSRand _fRandom)
      : p{_p}, fRandom{_fRandom} {}

  uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override {
    return fRandom(p, se);
  }

  // personalization of validRandomRandom, if necessary!
  uptr<Move<XES, XEv, XSH>> validRandomMove(const XES& se) override {
    return fValidRandom(p, se);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NS<XES>::idComponent() << ":FNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template specialization for empty problem = void

template <XESolution XES>
class FNS<XES, void> final : public NS<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(const XES&);
  typedef uptr<Move<XES>> (*FuncTypeNSValidRand)(const XES&);
#else
  typedef std::function<uptr<Move<XES>>(const XES&)> FuncTypeNSRand;
  typedef FuncTypeNSRand FuncTypeNSValidRand;  // same type
#endif

  FuncTypeNSRand fRandom;

  // standard implementation for fValidRandom for ProblemType=void
  FuncTypeNSValidRand fValidRandom{[this](const XES& se) -> uptr<Move<XES>> {
    uptr<Move<XES, XEv, XSH>> moveValid = this->fRandom(se);
    if (moveValid && moveValid->canBeApplied(se))
      return moveValid;
    else
      return nullptr;
  }};

  explicit FNS(FuncTypeNSRand _fRandom) : fRandom{_fRandom} {}

  uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override {
    return fRandom(se);
  }

  // personalization of validRandomRandom, if necessary!
  uptr<Move<XES, XEv, XSH>> validRandomMove(const XES& se) override {
    return fValidRandom(se);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NS<XES>::idComponent() << ":FNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FNS_HPP_
