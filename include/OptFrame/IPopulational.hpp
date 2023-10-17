// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_IPOPULATIONAL_HPP_
#define OPTFRAME_IPOPULATIONAL_HPP_

#include <string>
//
#include <OptFrame/GlobalSearch.hpp>
#include <OptFrame/Helper/EPopulation.hpp>
#include <OptFrame/StopCriteria.hpp>

// IMPORTANT: This file has IPopulational and Populational
// Populational inherits from GlobalSearch, while IPopulational does not

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES>
#else
template <typename XES, typename XSH = XES, typename XES2 = XES>
#endif
class IPopulational {  // do not inherit here!
  using XEv = typename XES::second_type;
  // this uses EPopulation, not "legacy Population"
  using IPopIncumbentType = optframe::EPopulation<XES2>;
  using IPopBestType = XSH;

 public:
  //
  // virtual method with search signature for populational methods
  //
  virtual SearchOutput<XES, IPopBestType> searchPopulational(
      std::optional<IPopBestType>& _best, IPopIncumbentType& _inc,
      const StopCriteria<XEv>& stopCriteria) = 0;
};

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES,
          XSearch<XES> XSH2 = XSH>
#else
template <typename XES, typename XSH = XES, typename XES2 = XES,
          typename XSH2 = XSH>
#endif
class Populational : public GlobalSearch<XES, XSH> {
  using XEv = typename XES::second_type;
  using BestType = XSH;

 public:
  using IncumbentType =
      optframe::VEPopulation<XES2>;  // this uses EPopulation, not "legacy
                                     // Population"

  virtual ~Populational() = default;

 public:
  // onIncumbent now will depend on Incumbent type (look on ITrajectory or
  // IPopulational)
  //
  bool (*onIncumbent)(GlobalSearch<XES, XSH>& self,
                      const IncumbentType& incumbent) =
      [](GlobalSearch<XES, XSH>& self, const IncumbentType& incumbent) {
        return true;
      };
  //

  // global search method (maybe, someday, create some abstract IGlobalSearch..
  // not now)
  // virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
  //
  // SearchOutput<XES, BestType> search(
  //    const StopCriteria<XEv>& stopCriteria) override = 0;

  //
  // virtual method with search signature for populational methods
  //
  virtual SearchOutput<XES, BestType> searchPopulational(
      std::optional<BestType>& _best, IncumbentType& _inc,
      const StopCriteria<XEv>& stopCriteria) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (GlobalSearch<XES, XSH>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearch<XES, XSH>::idComponent() << ":Populational"
       << Domain::getAlternativeDomain<XES>("<XESf64>") << ":";
    // NOTE THAT:
    // - PRIMARY/BEST IS TYPICALLY XESf64
    // - SECONDARY/INCUMBENT IS TYPICALLY X2ESf64
    // FOR NOW, WE DONT FLAG SECONDARY TYPES... BUT WE COULD IN THE FUTURE (IF
    // NECESSARY!) A NOTATION COULD BE: Populational<XESf64, X2ESf64>
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif  // OPTFRAME_IPOPULATIONAL_HPP_
