// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FDECODEROPRK_HPP_
#define OPTFCORE_EA_RK_FDECODEROPRK_HPP_

#include <functional>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFCore/FDirection.hpp>
#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp>

#include "OptFrame/Concepts/MyConcepts.hpp"

namespace optframe {

template <XESolution XES, ConceptsComparability KeyType, MinOrMax Minimizing>
class FDecoderOpRK final : public DecoderRandomKeys<XES, KeyType> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef pair<XEv, op<S>> (*FuncTypeDecodeOp)(const RSK& rk, bool);
#else
  typedef std::function<pair<XEv, op<S>>(const RSK&, bool)> FuncTypeDecodeOp;
#endif

  FuncTypeDecodeOp fDecodeOp;

  explicit FDecoderOpRK(FuncTypeDecodeOp _fDecode) : fDecodeOp{_fDecode} {}

  virtual ~FDecoderOpRK() = default;

  pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    auto p = fDecodeOp(rk, needsSolution);
    return p;
  }

  bool isMinimization() const override {
    return Minimizing == MinOrMax::MINIMIZE;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeys<XES, KeyType>::idComponent() << ":FDecoderOpRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_RK_FDECODEROPRK_HPP_
