// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FDECODERNOEVALUATIONRK_HPP_
#define OPTFCORE_EA_RK_FDECODERNOEVALUATIONRK_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <functional>
#include <string>
#include <vector>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeysNoEvaluation.hpp>

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

MOD_EXPORT template <XSolution S, ConceptsComparability KeyType>
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
