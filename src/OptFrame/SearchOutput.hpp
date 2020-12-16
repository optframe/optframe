#pragma once

#include "BaseConcepts.hpp"
#include "SearchStatus.hpp"

namespace optframe {

template<XESolution XES, XSearch<XES> XSH = XES>
class SearchOutput
{
public:
   SearchStatus status;
   std::optional<XSH> output;

}; // SearchOutput

} // namespace optframe