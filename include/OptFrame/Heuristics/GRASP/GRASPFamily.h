// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GRASP_FAMILY_H_
#define OPTFRAME_GRASP_FAMILY_H_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <string>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

// GRASPH ? TODO FIX THE NAME

namespace optframe {

MOD_EXPORT class GRASP {
 public:
  static std::string family() { return "GRASP"; }

  virtual ~GRASP() {}
};

}  // namespace optframe

#endif /*OPTFRAME_GRASP_FAMILY_H_*/
