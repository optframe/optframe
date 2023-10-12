// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FDECODERNOEVALUATIONRK_HPP_
#define OPTFCORE_EA_RK_FDECODERNOEVALUATIONRK_HPP_

#include <functional>
#include <string>
#include <vector>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeysNoEvaluation.hpp>

namespace optframe {

template <XSolution S, optframe::comparability KeyType>
class FDecoderNoEvaluationRK final
    : public DecoderRandomKeysNoEvaluation<S, KeyType> {
 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S (*FuncTypeDecode)(const std::vector<KeyType>& rk);
#else
  typedef std::function<S(const std::vector<KeyType>&)> FuncTypeDecode;
#endif

  FuncTypeDecode fDecode;

  explicit FDecoderNoEvaluationRK(FuncTypeDecode _fDecode)
      : fDecode{_fDecode} {}

  virtual ~FDecoderNoEvaluationRK() = default;

  S decodeSolution(const std::vector<KeyType>& rk) override {
    auto s = fDecode(rk);
    return s;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeysNoEvaluation<S, KeyType>::idComponent()
       << ":FDecoderNoEvaluationRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_RK_FDECODERNOEVALUATIONRK_HPP_
