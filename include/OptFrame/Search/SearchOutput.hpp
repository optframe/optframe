// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SEARCH_SEARCHOUTPUT_HPP_
#define OPTFRAME_SEARCH_SEARCHOUTPUT_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Concepts/BaseConcepts.hpp>

#include "SearchStatus.hpp"

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

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
struct SearchOutput {
  SearchStatus status;
  std::optional<XSH> best;
  double timeBest{-1.0};  // time to find 'best'

  // two real elements (copy best)
  SearchOutput(SearchStatus status, std::optional<XSH>& best,
               double timeBest = -1)
      : status{status}, best{best}, timeBest{timeBest} {}

  // move semantics or nothing (DO NOT MAKE IT explicit!)
  // NOLINTNEXTLINE
  SearchOutput(SearchStatus status, std::optional<XSH>&& best = std::nullopt,
               double timeBest = -1)
      : status{status}, best{best}, timeBest{timeBest} {}

};  // SearchOutput
}  // namespace optframe

#endif  // OPTFRAME_SEARCH_SEARCHOUTPUT_HPP_
