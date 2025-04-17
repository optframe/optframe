// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FDECODERRK_HPP_
#define OPTFCORE_EA_RK_FDECODERRK_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <functional>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFCore/FDirection.hpp>
#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XESolution XES, ConceptsComparability KeyType,
                     MinOrMax Minimizing>
class FDecoderRK final : public DecoderRandomKeys<XES, KeyType> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;

 public:
  // pair<XEv, S> (*fDecode)(const RSK& rk); // decode function

#ifdef OPTFCORE_FUNC_STATIC
  typedef std::pair<XEv, S> (*FuncTypeDecode)(const RSK& rk);
#else
  typedef std::function<std::pair<XEv, S>(const RSK&)> FuncTypeDecode;
#endif

  FuncTypeDecode fDecode;

  explicit FDecoderRK(FuncTypeDecode _fDecode) : fDecode{_fDecode} {}

  virtual ~FDecoderRK() = default;

  std::pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    auto p = fDecode(rk);
    return make_pair(p.first, make_optional(p.second));
  }

  bool isMinimization() const override {
    return Minimizing == MinOrMax::MINIMIZE;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeys<XES, KeyType>::idComponent() << ":FDecoderRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_RK_FDECODERRK_HPP_
