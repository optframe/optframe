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

#ifndef OPTFRAME_NSFIND_HPP_
#define OPTFRAME_NSFIND_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <OptFrame/Component.hpp>
#include <OptFrame/Core/GeneralEvaluator.hpp>  // included for Neighborhood Exploration
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NS.hpp>

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

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class NSFind : public NS<XES, XSH> {
 public:
  using XEv = typename XES::second_type;
  //
  virtual ~NSFind() = default;

 public:
  // =======================================
  // find section (neighborhood exploration)
  // =======================================
  // findBest: returns move that greatly improves current solution 'se',
  // according 'gev' RETURNS: pair< uptr<Move<XES, XSH>>, op<XEv> > NSFind
  // is useful for exponential-sized neighborhoods, without requiring any
  // iterator structure
  virtual std::pair<Move<XES, XSH>*, op<XEv>> findBest(
      GeneralEvaluator<XES, XSH>& gev, XES& se) = 0;

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << NS<XES, XSH>::idComponent() << ":NSFind";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NS<XES, XSH>::compatible(s));
  }
};

}  // namespace optframe

#endif /*OPTFRAME_NSFIND_HPP_*/
