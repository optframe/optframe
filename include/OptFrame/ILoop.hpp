// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ILOOP_HPP_  // NOLINT
#define OPTFRAME_ILOOP_HPP_

#include <OptFrame/SearchOutput.hpp>
#include <OptFrame/StopCriteria.hpp>

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

template <class SCTX, XESolution XES, XSearch<XES> XSH = XES>
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
