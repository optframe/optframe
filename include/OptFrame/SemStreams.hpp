// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SEMSTREAMS_HPP_
#define OPTFRAME_SEMSTREAMS_HPP_

// This is a fundamental class for providing Semantic Streams on OptFrame
// The stream indicates its purpose, so formatting can change according to
// stream

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <iostream>
#include <sstream>
#include <string>
//
#include <OptFrame/modlog/modlog.hpp>

#define MOD_EXPORT
#else

// ERROR: post-module-declaration imports must not be from header inclusion
// import std;

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT inline modlog::SemStream
    cjson{};  // (C++17 extern linkage is implicit)
MOD_EXPORT inline modlog::SemStream
    ctxt{};  // (C++17 extern linkage is implicit)
// MOD_EXPORT inline modlog::NullSemStream osNull{};  // (C++17 extern linkage
// is implicit)

}  // namespace optframe

#endif  // OPTFRAME_SEMSTREAMS_HPP_
