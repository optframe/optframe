// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2024 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_UTIL_TEST_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_TEST_HPP_
#define TESTS_OPTFRAME_TEST_UTIL_TEST_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_TEST_HPP_  // NOLINT

// C++ system
#include <limits>
#include <utility>
#include <vector>
//
// #include <catch2/catch_amalgamated.hpp>
#ifdef MAKE
#include <catch2/catch_amalgamated.hpp>
#else
#include <catch2/catch_test_macros.hpp>
#endif

// core includes
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRP2Opt.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

class CVRPProblemTestNSSeq2Opt {};

TEST_CASE("OptFrameUtilTests: NSSeqVRP2Opt") {
  using Routes = std::vector<std::vector<int>>;
  using ESolutionCVRPTest = std::pair<Routes, Evaluation<int>>;

  CVRPProblemTestNSSeq2Opt p;

  NSSeqVRP2Opt<ESolutionCVRPTest, CVRPProblemTestNSSeq2Opt> nsseq_v1{
      [](const ESolutionCVRPTest& se) -> Routes& {
        // NOLINTNEXTLINE
        auto& r = const_cast<Routes&>(se.first);
        return r;
      },
      &p};

  REQUIRE(nsseq_v1.toString() ==
          "NSSeqVRP2Opt with move: OptFrame:Move:MoveVRP2Opt");

  // #ifndef _MSC_VER
  NSSeqVRP2Opt<ESolutionCVRPTest, CVRPProblemTestNSSeq2Opt> nsseq_v2;

  REQUIRE(nsseq_v2.toString() ==
          "NSSeqVRP2Opt with move: OptFrame:Move:MoveVRP2Opt");
  // #endif
}

#endif  // TESTS_OPTFRAME_TEST_UTIL_TEST_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_TEST_HPP_
        // // NOLINT
