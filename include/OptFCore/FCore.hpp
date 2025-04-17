// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FCORE_HPP_
#define OPTFCORE_FCORE_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>

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

// OptFrame Functional Core (FCore)
MOD_EXPORT class FCore {
 public:
  static constexpr char OPTFRAME_VERSION[] = "5.1.0";
  static std::string version() { return OPTFRAME_VERSION; }

  static std::string welcome() {
    std::string s =
        "Welcome to OptFrame Functional Core (FCore) - version " + version();
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    s.append(" (built with c++20 concepts)");
#else
    s.append(" (built without c++20 concepts)");
#endif
    return s;
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FCORE_HPP_
