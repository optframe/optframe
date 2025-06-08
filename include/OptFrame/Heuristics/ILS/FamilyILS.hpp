// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ILS_FAMILY_HPP_
#define OPTFRAME_ILS_FAMILY_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string.h>

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

MOD_EXPORT class FamilyILS {
 public:
  static std::string family() { return "ILS:"; }

  virtual ~FamilyILS() {}
};

}  // namespace optframe

#endif /*OPTFRAME_ILS_FAMILY_HPP_*/
