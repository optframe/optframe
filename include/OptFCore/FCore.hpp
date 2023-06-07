// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FCORE_HPP_
#define OPTFCORE_FCORE_HPP_

#include <string>

// Functional Core Module: inclusion of "Functional OptFrame Core" components

#include <OptFCore/FConstructive.hpp>
#include <OptFCore/FDirection.hpp>
#include <OptFCore/FEvaluator.hpp>
#include <OptFCore/FMove.hpp>
#include <OptFCore/FNS.hpp>
#include <OptFCore/FNSSeq.hpp>
//
#include "EA/RK/FConstructiveRK.hpp"         // Extra
#include "EA/RK/FDecoderEvalRK.hpp"          // Extra
#include "EA/RK/FDecoderNoEvaluationRK.hpp"  // Extra
#include "EA/RK/FDecoderRK.hpp"              // Extra
//
#include <OptFCore/EA/FGeneralCrossover.hpp>

namespace optframe {

// OptFrame Functional Core (FCore)
class FCore {
 public:
  static constexpr char OPTFRAME_VERSION[] = "5.0.10";
  static std::string version() { return OPTFRAME_VERSION; }

  static std::string welcome() {
    return "Welcome to OptFrame Functional Core (FCore) - version " + version();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FCORE_HPP_
