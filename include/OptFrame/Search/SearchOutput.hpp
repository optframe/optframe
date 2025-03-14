#pragma once

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

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

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class SearchOutput {
 public:
  SearchStatus status;
  std::optional<XSH> best;

  // two real elements (copy best)
  SearchOutput(SearchStatus status, std::optional<XSH>& best)
      : status{status}, best{best} {}

  // move semantics or nothing (DO NOT MAKE IT explicit!)
  // NOLINTNEXTLINE
  SearchOutput(SearchStatus status, std::optional<XSH>&& best = std::nullopt)
      : status{status}, best{best} {}
};  // SearchOutput

}  // namespace optframe
