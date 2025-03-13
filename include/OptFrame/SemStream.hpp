// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SEMSTREAM_HPP_
#define OPTFRAME_SEMSTREAM_HPP_

// This is a fundamental class for providing Semantic Streams on OptFrame
// The stream indicates its purpose, so formatting can change according to
// stream

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <iostream>
#include <sstream>
#include <string>

#define MOD_EXPORT
#else

import std;

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

// semantic streams (optframe::cjson, ctxt, etc)
MOD_EXPORT struct SemStream : private std::streambuf, public std::ostream {
 private:
  // possibly storing locally
  std::stringstream oss;

 public:
  // just a raw pointer
  std::ostream* os;

  explicit SemStream(std::ostream& _os) : std::ostream{this}, os{&_os} {}

  SemStream() : std::ostream{this}, os{&oss} {}

 private:
  int overflow(int c) override {
    (*os).put(c);
    return 0;
  }

 public:
  void setStream(std::ostream& _os) { os = &_os; }

  std::string dump() {
    std::string dump{oss.str()};
    // TODO: do we need os->clear() ?
    oss.str(std::string());  // better than str("")
    os->clear();
    return dump;
  }
};

//  null semantic streams
struct NullSemStream : private std::streambuf, public std::ostream {
 private:
  // nothing stored locally

 public:
  // ignore parameter
  explicit NullSemStream(std::ostream& _os) : std::ostream{this} {}
  NullSemStream() : std::ostream{this} {}

 private:
  int overflow(int c) override { return c; }

 public:
  // ignore
  void setStream(std::ostream& _os) {}

  std::string dump() {
    return "";  // always empty
  }
};

// extern SemStream cjson; // only in .cpp
// extern SemStream ctxt;  // only in .cpp
MOD_EXPORT inline SemStream cjson{};       // (C++17 extern linkage is implicit)
MOD_EXPORT inline SemStream ctxt{};        // (C++17 extern linkage is implicit)
MOD_EXPORT inline NullSemStream osNull{};  // (C++17 extern linkage is implicit)

}  // namespace optframe

#endif  // OPTFRAME_SEMSTREAM_HPP_
