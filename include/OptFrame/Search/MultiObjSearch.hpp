// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIOBJSEARCH_HPP_
#define OPTFRAME_MULTIOBJSEARCH_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// C++
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/Direction.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/MultiEvaluation.hpp>
#include <OptFrame/Core/MultiEvaluator.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Pareto/Pareto.hpp>
#include <OptFrame/Pareto/ParetoDominance.hpp>
#include <OptFrame/Pareto/ParetoDominanceWeak.hpp>
#include <OptFrame/Search/GlobalSearch.hpp>  // Base class

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

// using namespace std;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <class Self, class XES>
concept XMultiObjSearch = requires(Self a) {
  requires XESolution<XES>;
  requires X2ESolution<typename Self::BestType, XES>;
};
#endif  // cpp_concepts

// EASY ON XMES2... FOR NOW! NOT REQUIRING XEMSolution, BUT WE SHOULD...
// MUST FIX IndividualNSGAII and other problematic implementations...
// BASE TYPE XMES IS NOW FINALLY REQUIRED TO BE XEMSolution!
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XEMSolution XMES, XESolution XMES2 = XMES,
                     XSearch<XMES2> XMSH2 = XMES2>
#else
MOD_EXPORT template <typename XMES, typename XMES2 = XMES,
                     typename XMSH2 = XMES2>
#endif
class MultiObjSearch : public GlobalSearch<XMES, Pareto<XMES>> {
  using XMSH = Pareto<XMES>;  // PRIMARY/BEST search space
  //
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  static_assert(XEvaluation<typename XMEv::XEv>);
#endif
  using XEv = typename XMEv::XEv;
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  static_assert(XEvaluation<XEv>);
#endif

 public:
  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XMultiObjSearch
  // ========================================

  MultiObjSearch() {}

  virtual ~MultiObjSearch() = default;

  op<Pareto<XMES>>& getBestPareto() { return this->best; }

  SearchOutput<XMES, Pareto<XMES>> searchBy(
      const StopCriteria<XMEv>& stopCriteria,
      std::optional<Pareto<XMES>> _best) override = 0;

  std::string log() const override { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << "MultiObjSearch"
       << Domain::getAlternativeDomain<XMSH>("<X2MESf64>") << ":";
    // NOTE THAT: PRIMARY/BEST IS ALWAYS X2MESf64 FOR MULTI-OBJ-SEARCH
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_MULTIOBJSEARCH_HPP_
