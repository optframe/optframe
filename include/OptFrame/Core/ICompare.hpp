// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ICOMPARE_HPP_
#define OPTFRAME_ICOMPARE_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <OptFrame/Concepts/BaseConcepts.hpp>

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

// Allows Single (XEv) and Multi (XMEv)
MOD_EXPORT template <XEvaluation XEv>
class ICompare  // NO INHERITANCE HERE!
{
 public:
  // 'equals' is offered by GeneralEvaluator! DO NOT PUT IT HERE!!
  // virtual bool equals(const XEv& e1, const XEv& e2) = 0;
  virtual bool betterThan(const XEv& e1, const XEv& e2) = 0;
  // no 'betterOrEquals' here (since there's no 'equals'...)
  // virtual bool betterOrEquals(const XEv& e1, const XEv& e2) = 0;

  // This is inherited by Direction and Evaluator
};

}  // namespace optframe

#endif  // OPTFRAME_ICOMPARE_HPP_
