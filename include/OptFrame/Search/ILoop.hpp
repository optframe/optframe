// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ILOOP_HPP_  // NOLINT
#define OPTFRAME_ILOOP_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <OptFrame/Search/SearchOutput.hpp>
#include <OptFrame/Search/StopCriteria.hpp>

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

// This is meant for Loop-Based metaheuristics
// Includes methods:
// - onLoop(ctx, stop)
// - onBeforeLoop(ctx)

namespace optframe {

// 'SCTX' defines the method search context, such as local variables
// and other control mechanisms that can be personalized.
// Defines callbacks:
// - onLoop(SCTX, StopCriteria)
// - onUpdateLoop(SCTX)

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <class SCTX, XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <class SCTX, typename XES, typename XSH = XES>
#endif
class ILoop  // do not inherit here!
{
  using XEv = typename XES::second_type;

 public:
  // mandatory onLoop callback
  virtual bool onLoop(const SCTX& ctx, const StopCriteria<XEv>& stop) = 0;

  // mandatory onBeforeLoop callback
  virtual void onBeforeLoop(SCTX& ctx) = 0;
};

}  // namespace optframe

#endif  // OPTFRAME_ILOOP_HPP_ // NOLINT
