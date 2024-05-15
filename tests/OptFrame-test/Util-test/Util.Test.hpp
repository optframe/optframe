// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_UTIL_TEST_UTIL_TEST_HPP_
#define TESTS_OPTFRAME_TEST_UTIL_TEST_UTIL_TEST_HPP_

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

#include "./NSAdapter/VRP/Inter/NSSeqVRPShift10.Test.hpp"
#include "./NSAdapter/VRP/Intra/NSSeqVRP2Opt.Test.hpp"
#include "./NSAdapter/VRP/Intra/NSSeqVRPExchange.Test.hpp"
#include "./NSAdapter/VRP/Intra/NSSeqVRPOrOpt1.Test.hpp"
#include "./NSAdapter/VRP/Intra/NSSeqVRPOrOpt2.Test.hpp"

#endif  // TESTS_OPTFRAME_TEST_UTIL_TEST_UTIL_TEST_HPP_
