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

#ifndef OPTFRAME_FCORE_FDECODER_RK_HPP_
#define OPTFRAME_FCORE_FDECODER_RK_HPP_

#include <OptFrame/Heuristics/EA/RK/DecoderRandomKeys.hpp>
#include <functional>

namespace optframe {

template <
    XESolution XES,
    optframe::comparability KeyType,
    MinOrMax Minimizing>
class FDecoderRK final : public DecoderRandomKeys<XES, KeyType> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;

 public:
  //pair<XEv, S> (*fDecode)(const RSK& rk); // decode function

#ifdef OPTFCORE_FUNC_STATIC
  typedef pair<XEv, S> (*FuncTypeDecode)(const RSK& rk);
#else
  typedef std::function<pair<XEv, S>(const RSK&)> FuncTypeDecode;
#endif

  FuncTypeDecode fDecode;

  FDecoderRK(FuncTypeDecode _fDecode)
      : fDecode{_fDecode} {
  }

  virtual ~FDecoderRK() {
  }

  virtual pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    auto p = fDecode(rk);
    return make_pair(p.first, make_optional(p.second));
  }

  virtual bool isMinimization() const override {
    return Minimizing == MinOrMax::MINIMIZE;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << DecoderRandomKeys<XES, KeyType>::idComponent() << ":FDecoderRK";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_FCORE_FDECODER_RK_HPP_*/
