// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FCONSTRUCTIVE_HPP_
#define OPTFCORE_FCONSTRUCTIVE_HPP_

// c++
#include <functional>
#include <string>
// OptFrame
#include <OptFrame/Constructive.hpp>
//

namespace optframe {

template <XSolution S>
class FConstructive final : public Constructive<S> {
  using super = Constructive<S>;

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef S(*FuncTypeGenerate);
#else
  typedef std::function<S()> FuncTypeGenerate;
#endif

  // S(*fGenerate)(); // constructive
  FuncTypeGenerate fGenerate;

  FConstructive(FuncTypeGenerate _fGenerate) : fGenerate{_fGenerate} {}

  virtual ~FConstructive() {}

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

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FCONSTRUCTIVE_HPP_
