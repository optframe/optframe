// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_EA_FAMILY_H_
#define OPTFRAME_EA_FAMILY_H_

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

// using namespace std;

namespace optframe {

MOD_EXPORT class FamilyEA {
 public:
  static std::string family() { return "EA"; }

  virtual ~FamilyEA() {}
};

}  // namespace optframe

#endif /*OPTFRAME_EA_FAMILY_H_*/
