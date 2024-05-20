// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HELPER_TEST_MULTIESOLUTION_TEST_HPP_
#define TESTS_OPTFRAME_TEST_HELPER_TEST_MULTIESOLUTION_TEST_HPP_

// C++
#include <limits>
#include <string>
#include <utility>
#include <vector>

// testing
// #include <catch2/catch_amalgamated.hpp>
#ifdef MAKE
#include <catch2/catch_amalgamated.hpp>
#else
#include <catch2/catch_test_macros.hpp>
#endif

// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// core includes
#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame-Core-Examples/FCore-bTSP/BTSP-fcore.hpp>
// #include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
#include <OptFrame/Helper/MultiESolution.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

TEST_CASE("OptFrameHelperTests: multiesolution") {
  using namespace BTSP_fcore;

  MultiESolution<ESolutionBTSP> me;

  REQUIRE(me.size() == 0);
}

#endif  // TESTS_OPTFRAME_TEST_HELPER_TEST_MULTIESOLUTION_TEST_HPP_
