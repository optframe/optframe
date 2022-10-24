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

#ifndef OPTFRAME_FCORE_FDECODER_NOEVALUATION_RK_HPP_
#define OPTFRAME_FCORE_FDECODER_NOEVALUATION_RK_HPP_

#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeysNoEvaluation.hpp>
#include <functional>

namespace optframe {

template <XSolution S, optframe::comparability KeyType>
class FDecoderNoEvaluationRK final : public DecoderRandomKeysNoEvaluation<S, KeyType> {
 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S (*FuncTypeDecode)(const std::vector<KeyType>& rk);
#else
  typedef std::function<S(const std::vector<KeyType>&)> FuncTypeDecode;
#endif

  FuncTypeDecode fDecode;

  FDecoderNoEvaluationRK(FuncTypeDecode _fDecode)
      : fDecode{_fDecode} {
  }

  virtual ~FDecoderNoEvaluationRK() {
  }

  virtual S decodeSolution(const std::vector<KeyType>& rk) override {
    auto s = fDecode(rk);
    return s;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeysNoEvaluation<S, KeyType>::idComponent() << ":FDecoderNoEvaluationRK";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_FCORE_FDECODER_NOEVALUATION_RK_HPP_*/
