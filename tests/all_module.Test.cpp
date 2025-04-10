// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2025 - OptFrame developers
// https://github.com/optframe/optframe

#include <catch2/catch_test_macros.hpp>

import std;
import optframe;
import optframe.functional;
import optframe.examples.kp;
import optframe.examples.btsp;

// core includes
// #include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
// #include <OptFrame-Core-Examples/FCore-bTSP/BTSP-fcore.hpp>
// #include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
// #include <OptFrame/Core/MultiESolution.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

TEST_CASE("OptFrameHelperTests: multiesolution") {
  using namespace BTSP_fcore;

  MultiESolution<ESolutionBTSP> me;

  REQUIRE(me.size() == 0);
}
