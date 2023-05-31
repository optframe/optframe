// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ITRAJECTORY_HPP_
#define OPTFRAME_ITRAJECTORY_HPP_

#include "SearchOutput.hpp"
#include "StopCriteria.hpp"

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
// for trajectory methods, XSH2 = XSH
template <XESolution XES, XSearch<XES> XSH = XES>
class ITrajectory  // do not inherit here!
{
  using XEv = typename XES::second_type;

 public:
  // global search method (maybe, someday, create some abstract IGlobalSearch..
  // not now)
  virtual SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& stopCriteria,
                                          std::optional<XSH> best) = 0;

  // virtual method with search signature for trajectory methods
  virtual SearchOutput<XES, XSH> searchByIncumbent(
      XSH& _best, XSH& _inc, const StopCriteria<XEv>& stopCriteria) = 0;
};

}  // namespace optframe

#endif /* OPTFRAME_ITRAJECTORY_HPP_ */
