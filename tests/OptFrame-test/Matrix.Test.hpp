// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_MATRIX_TEST_HPP_  // NOLINT
#define TESTS_OPTFRAME_TEST_MATRIX_TEST_HPP_  // NOLINT

// C++ system
#include <limits>
//
#include <catch2/catch_amalgamated.hpp>

// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// core includes
#include <OptFrame/Util/Matrix.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("OptFrameUtilsTests: Matrix_Is_Zero") {
  // BigInteger big;
  // EXPECT_EQ(big.Length(), 1);
  Matrix<int> m(2);
  REQUIRE(m.getNumRows() == 2);
}

#endif  // TESTS_OPTFRAME_TEST_MATRIX_TEST_HPP_ // NOLINT
