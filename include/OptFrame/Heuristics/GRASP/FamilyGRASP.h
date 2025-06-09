// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GRASPH_H_
#define OPTFRAME_GRASPH_H_

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

class FamilyGRASP {
 public:
  static std::string family() { return "GRASP:"; }

  virtual ~FamilyGRASP() {}
};

}  // namespace optframe

#endif /*OPTFRAME_GRASPH_H_*/
