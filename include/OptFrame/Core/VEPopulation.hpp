// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_VEPOPULATION_HPP_
#define OPTFRAME_VEPOPULATION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <vector>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>

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
//

// ================================================================
// Remember that VEPopulation is NOT a optframe::Component!
//
// This was created just to simplify development, using std::vector
// as the container for a std::pair of solution and evaluation.
// It implements concept X2ESolution.
//
// If you need optframe::Component, try EPopulation or "legacy" Population
// Also, you can try MultiESolution, as a "neutral" alternative (semantically
// free)
//

namespace optframe {

MOD_EXPORT template <XSolution S>
using VPopulation = std::vector<S>;

MOD_EXPORT template <XESolution XES>
using VEPopulation = std::vector<XES>;

}  // namespace optframe

// NO TEST HERE!
/*
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

#include "VEPopulation.test.hpp"

#endif  // cpp_concepts
*/

#endif /* OPTFRAME_VEPOPULATION_HPP_ */
