#pragma once

#include <OptFrame/Concepts/BaseConcepts.hpp>

#include "SearchStatus.hpp"

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
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
