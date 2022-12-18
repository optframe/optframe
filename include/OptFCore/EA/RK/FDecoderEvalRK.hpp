// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

template <XESolution XES, optframe::comparability KeyType>
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
