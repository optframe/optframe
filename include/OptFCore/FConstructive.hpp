// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FCONSTRUCTIVE_HPP_
#define OPTFCORE_FCONSTRUCTIVE_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// c++
#include <functional>
#include <string>
// OptFrame
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Core/Constructive.hpp>
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


MOD_EXPORT namespace optframe {

template <XSolution S, typename ProblemType = void>
class FConstructive final : public Constructive<S> {
  using super = Constructive<S>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S (*FuncTypeGenerate)(sref<ProblemType>);
#else
  typedef std::function<S(sref<ProblemType>)> FuncTypeGenerate;
#endif

  // S(*fGenerate)(); // constructive
  sref<ProblemType> p;
  FuncTypeGenerate fGenerate;

  FConstructive(sref<ProblemType> _p, FuncTypeGenerate _fGenerate)
      : p{_p}, fGenerate{_fGenerate} {}

  virtual ~FConstructive() = default;

  // fGenerate is supposed to be the simplest possible function.
  // Although fGenerate does not require optional or timelimit (two
  // connected concepts),
  // the Constructive class must support... since it's completely general.
  //
  std::optional<S> generateSolution(double timelimit) override {
    return std::make_optional(fGenerate(p));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Constructive<S>::idComponent() << ":FConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template specialization for empty problem = void

template <XSolution S>
class FConstructive<S, void> final : public Constructive<S> {
  using super = Constructive<S>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S(*FuncTypeGenerate);
#else
  typedef std::function<S()> FuncTypeGenerate;
#endif

  // S(*fGenerate)(); // constructive
  FuncTypeGenerate fGenerate;

  explicit FConstructive(FuncTypeGenerate _fGenerate) : fGenerate{_fGenerate} {}

  virtual ~FConstructive() = default;

  // fGenerate is supposed to be the simplest possible function.
  // Although fGenerate does not require optional or timelimit (two
  // connected concepts),
  // the Constructive class must support... since it's completely general.
  //
  std::optional<S> generateSolution(double timelimit) override {
    return std::make_optional(fGenerate());
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Constructive<S>::idComponent() << ":FConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FCONSTRUCTIVE_HPP_
