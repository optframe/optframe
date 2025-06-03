// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2023 - OptFrame - https://github.com/optframe/optframe

#define NO_CXX_MODULES 1  // disable CXX_MODULES for optframe

// C++
// #include <iostream>
#include <cstdint>
#include <vector>
//
// #include <OptFCore/FCore.hpp> // included src
#include <OptFCore/FCore.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Core/EPopulation.hpp>
#include <OptFrame/Core/Evaluation.hpp>

using optframe::Evaluation;

// simple test for package loading (on bazel)

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS

using optframe::X2ESolution;
using optframe::XESolution;
#endif

using ESolutionTest = std::pair<int, Evaluation<>>;

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
static_assert(XESolution<ESolutionTest>);
#endif

using EPopulationTest = std::vector<ESolutionTest>;

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
static_assert(X2ESolution<EPopulationTest, ESolutionTest>);
#endif

using EPopulationRealTest = optframe::EPopulation<ESolutionTest>;

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
static_assert(X2ESolution<EPopulationRealTest, ESolutionTest>);
#endif

int main() {
  EPopulationTest epop;
  std::cout << "Hello OptFrame" << std::endl;
  //
  std::cout << optframe::FCore::welcome() << std::endl;
  //
  optframe::Evaluation<int> ev;
  //
  return 0;
}
