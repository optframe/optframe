// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DOMAIN_CTEST_HPP_
#define OPTFRAME_DOMAIN_CTEST_HPP_

// Provides standard (named) domains for optframe. Notably defines:
// <XS>, <XES>, <XRKf64>, <XMESf64>, <X2ESf64> and <X2MESf64>

#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
//
#include <OptFrame/Domain.hpp>
// important stuff TO TEST
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>
// for testing
#include <OptFrame/BasicPareto.hpp>
#include <OptFrame/Helper/EPopulation.hpp>
// #include <OptFrame/Helper/MultiESolution.hpp>
#include <OptFrame/Helper/MultiEvaluation.hpp>

// IMPORTANT STUFF TO TEST!

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
static_assert(Domain::getNamedDomain<void*>() == std::string_view{"<XS>"});
#else
static_assert(Domain::getNamedDomain<void*>() == std::string_view{"<X?>"});
#endif
//
static_assert(Domain::getNamedDomain<std::vector<float>>() ==
              std::string_view{"<XRKf32>"});
static_assert(Domain::getNamedDomain<std::vector<double>>() ==
              std::string_view{"<XRKf64>"});
using TestXRKf64Ef64 = std::pair<std::vector<double>, optframe::Evaluation<>>;
static_assert(Domain::getNamedDomain<TestXRKf64Ef64>() ==
              std::string_view{"<XRKf64Ef64>"});
// ------------------------
// DO NOT USE MultiESolution
// static_assert(Domain::getNamedDomain<MultiESolution<TestXRKf64Ef64>>() ==
//              std::string_view("<X2RKf64Ef64>"));
// -------------------------------------------
//
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<float>>>() ==
    std::string_view("<XESf32>"));
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<double>>>() ==
    std::string_view("<XESf64>"));
using TestXES1 = std::pair<void*, optframe::Evaluation<>>;
static_assert(Domain::getNamedDomain<TestXES1>() ==
              std::string_view("<XESf64>"));
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<int64_t>>>() ==
    std::string_view("<XESi64>"));
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<int>>>() ==
    std::string_view("<XES>"));
#else
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<int>>>() ==
    std::string_view("<X?>"));
#endif
// -------------------------------------------------------------------
// DO NOT USE: MultiESolution (BAD CLASS!): ONLY USE EPopulation NOW.
// static_assert(Domain::getNamedDomain<MultiESolution<TestXES1>>() ==
//               std::string_view("<X2ESf64>"));
//
static_assert(Domain::getNamedDomain<EPopulation<TestXES1>>() ==
              std::string_view("<X2ESf64>"));
using EMSolution_Test1 = std::pair<void*, MultiEvaluation<double>>;
static_assert(Domain::getNamedDomain<EMSolution_Test1>() ==
              std::string_view("<XMESf64>"));
static_assert(Domain::getNamedDomain<BasicPareto<EMSolution_Test1>>() ==
              std::string_view("<X2MESf64>"));
// #endif

}  // namespace optframe

#endif  // OPTFRAME_DOMAIN_HPP_
