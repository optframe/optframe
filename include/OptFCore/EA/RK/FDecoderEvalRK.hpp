// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FDECODEREVALRK_HPP_
#define OPTFCORE_EA_RK_FDECODEREVALRK_HPP_

// c++
#include <functional>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp>

namespace optframe {

template <XESolution XES, ConceptsComparability KeyType>
class FDecoderEvalRK final : public DecoderRandomKeys<XES, KeyType> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;

 public:
  sref<Evaluator<typename XES::first_type, typename XES::second_type, XES>>
      eval;
  // pair<XEv, S> (*fDecode)(const RSK& rk); // decode function

#ifdef OPTFCORE_FUNC_STATIC
  typedef pair<XEv, S> (*FuncTypeDecodeEval)(sref<Evaluator<XES>> eval,
                                             const RSK& rk);
#else
  typedef std::function<pair<XEv, S>(
      sref<Evaluator<typename XES::first_type, typename XES::second_type, XES>>
          eval,
      const RSK&)>
      FuncTypeDecodeEval;
#endif

  FuncTypeDecodeEval fDecodeEval;

  FDecoderEvalRK(
      sref<Evaluator<typename XES::first_type, typename XES::second_type, XES>>
          _eval,
      FuncTypeDecodeEval _fDecodeEval)
      : eval{_eval}, fDecodeEval{_fDecodeEval} {}

  virtual ~FDecoderEvalRK() = default;

  pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    auto p = fDecodeEval(this->eval, rk);
    return make_pair(p.first, make_optional(p.second));
  }

  bool isMinimization() const override { return eval->isMinimization(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeys<XES, KeyType>::idComponent() << ":FDecoderRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_RK_FDECODEREVALRK_HPP_
