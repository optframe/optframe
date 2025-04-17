// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_EA_RK_FCONSTRUCTIVERK_HPP_  // NOLINT
#define OPTFCORE_EA_RK_FCONSTRUCTIVERK_HPP_  // NOLINT

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <functional>
#include <string>
#include <vector>
//
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Heuristics/EA/EA.hpp>
#include <OptFrame/Heuristics/EA/RK/ConstructiveRK.hpp>
#include <OptFrame/Heuristics/EA/RK/RK.hpp>

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

// only works for RK representation
//
MOD_EXPORT template <class KeyType = double>
class FConstructiveRK final : public ConstructiveRK<KeyType> {
  using super = ConstructiveRK<KeyType>;

  using S = std::vector<KeyType>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S(*FuncTypeGenerate);
#else
  typedef std::function<S()> FuncTypeGenerate;
#endif

  // S(*fGenerate)(); // constructive
  FuncTypeGenerate fGenerate;

  // S (*_fGenerate)())
  // S (*_fGenerate)())
  explicit FConstructiveRK(FuncTypeGenerate _fGenerate)
      : fGenerate{_fGenerate} {
    // std::cout << "FConstructive()" << std::endl;
  }

  virtual ~FConstructiveRK() = default;

  // fGenerate is supposed to be the simplest possible function.
  // Although fGenerate does not require optional or timelimit (two connected
  // concepts), the Constructive class must support... since it's completely
  // general.
  //
  std::optional<S> generateSolution(double timelimit) override {
    return std::make_optional(fGenerate());
  }

  bool compatible(std::string s) override {
    // Experimental: trying substring matching
    // If MY id() INCLUDES 's' and position is ZERO, then we must accept it,
    // right?
    std::string sid = id();
    std::size_t found_begin = sid.find(s);
    if (found_begin == 0) return true;
    //
    return (s == idComponent()) ||
           (Constructive<std::vector<KeyType>>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << super::idComponent() << ":FConstructiveRK";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_EA_RK_FCONSTRUCTIVERK_HPP_ // NOLINT
