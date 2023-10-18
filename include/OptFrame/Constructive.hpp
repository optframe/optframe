// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONSTRUCTIVE_HPP_
#define OPTFRAME_CONSTRUCTIVE_HPP_

#include <string>
//

#include <OptFrame/Concepts/BaseConcepts.hpp>

#include "Component.hpp"
#include "Domain.hpp"

namespace optframe {

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

    ss << s << ":Constructive" 
    #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    << Domain::getAlternativeDomain<S>("<XS>");
    #else
    << Domain::getAlternativeDomain<S>("<X?>");
    #endif

    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};
//

#ifndef NDEBUG
// static_assert(Domain::getAlternativeDomain<void*>("<XS>") ==
// std::string_view(""));
// static_assert(Domain::getAlternativeDomain<std::vector<double>>("<XS>") ==
// std::string_view("<XRKf64>"));
#endif

}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_HPP_*/
