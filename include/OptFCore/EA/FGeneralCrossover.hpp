// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_FGENERALCROSSOVER_HPP_
#define OPTFCORE_EA_FGENERALCROSSOVER_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// c++
#include <functional>
#include <string>
#include <utility>
// OptFrame
#include <OptFrame/Heuristics/EA/GeneralCrossover.hpp>
//

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

MOD_EXPORT template <XSolution S>
class FGeneralCrossover final : public GeneralCrossover<S> {
  using super = GeneralCrossover<S>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef std::pair<S, S> (*FuncTypeCross)(const S&, const S&);
#else
  typedef std::function<std::pair<S, S>(const S&, const S&)> FuncTypeCross;
#endif

  FuncTypeCross fCross;

  FGeneralCrossover(FuncTypeCross _fCross) : fCross{_fCross} {}

  virtual ~FGeneralCrossover() {}

  std::pair<op<S>, op<S>> cross(const S& s1, const S& s2) override {
    auto p = fCross(s1, s2);
    std::pair<op<S>, op<S>> pop = {
        std::make_optional(p.first),
        std::make_optional(p.second),
    };
    return pop;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (super::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GeneralCrossover<S>::idComponent() << ":FGeneralCrossover";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_FGENERALCROSSOVER_HPP_
