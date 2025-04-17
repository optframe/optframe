// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_PARETODOMINANCE_WEAK_HPP_
#define OPTFRAME_PARETODOMINANCE_WEAK_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <cmath>
#include <iostream>
#include <utility>
//
#include <OptFrame/Pareto/MultiEvaluation.hpp>
#include <OptFrame/Pareto/ParetoDominance.hpp>

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

MOD_EXPORT template <XESolution XES, XEMSolution XMES>
class ParetoDominanceWeak : public ParetoDominance<XES, XMES> {
 public:
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;
  //
  using ParetoDominance<XES, XMES>::dominates;

  explicit ParetoDominanceWeak(sref<MultiEvaluator<XES, XMES>> _mev)
      : ParetoDominance<XES, XMES>(_mev) {}

  virtual ~ParetoDominanceWeak() {}

  virtual bool dominates(const XMEv& mev1, const XMEv& mev2) {
    std::pair<int, int> betterEquals =
        ParetoDominance<XES, XMES>::checkDominates(mev1, mev2);
    int better = betterEquals.first;
    int equals = betterEquals.second;

    return (((unsigned)better + equals == mev1.size()));
  }
};

}  // namespace optframe

#endif /*OPTFRAME_PARETODOMINANCE_WEAK_HPP_*/
