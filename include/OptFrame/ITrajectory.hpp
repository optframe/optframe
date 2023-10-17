// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ITRAJECTORY_HPP_
#define OPTFRAME_ITRAJECTORY_HPP_

#include <OptFrame/SearchOutput.hpp>
#include <OptFrame/StopCriteria.hpp>

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
// for trajectory methods, XSH2 = XSH
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class ITrajectory {
  using XEv = typename XES::second_type;

 public:
  using IncumbentType = XSH;

  // callback when a new incumbent solution starts processing
  bool (*onIncumbent)(ITrajectory<XES, XSH>& self, const IncumbentType& inc) =
      [](ITrajectory<XES, XSH>& self, const IncumbentType& inc) {
        return true;
      };

  // virtual method with search signature for trajectory methods
  //
  // REMOVED! Method 'searchByIncumbent' is meaningless if XBest = XIncumbent!!
  // virtual SearchOutput<XES, XSH> searchByIncumbent(
  //   XSH& _best, XSH& _inc, const StopCriteria<XEv>& stopCriteria) = 0;
};

}  // namespace optframe

#endif  // OPTFRAME_ITRAJECTORY_HPP_
