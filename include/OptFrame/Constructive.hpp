// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONSTRUCTIVE_HPP_
#define OPTFRAME_CONSTRUCTIVE_HPP_

#include <OptFrame/BaseConcepts.hpp>

#include "Component.hpp"
//#include "Solution.hpp"
//#include "Solutions/CopySolution.hpp"
//#include "InitialSearch.hpp" // TODO: remove

namespace optframe {

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R, ADS> S =
// CopySolution<R, ADS>>
template <XSolution S>
class Constructive : public Component {
 public:
  ~Constructive() override = default;

  // timelimit in seconds, accepting fractions (millisecs, ...)
  // may or may not generate valid solution in time
  virtual std::optional<S> generateSolution(double timelimit) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    std::string s = Component::idComponent();
    ss << s << ":Constructive";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};
//
}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_HPP_*/
