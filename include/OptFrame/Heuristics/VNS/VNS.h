// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_VNS_FAMILY_H_
#define OPTFRAME_VNS_FAMILY_H_

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

namespace optframe {

MOD_EXPORT class VNS {
 public:
  static std::string family() { return "VNS:"; }

  virtual ~VNS() {}
};

}  // namespace optframe

#endif /*OPTFRAME_VNS_FAMILY_H_*/
