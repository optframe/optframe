// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SA_SA_HPP_
#define OPTFRAME_HEURISTICS_SA_SA_HPP_

#include <string>

// SA Family

namespace optframe {

// This is an interface, but please do not inherit this!
// Use XFamily concept-checking instead!
class SA {
 public:
  static std::string family() { return "SA:"; }

  // virtual ~SA() {}
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
static_assert(XFamily<SA>);
#endif

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SA_SA_HPP_
