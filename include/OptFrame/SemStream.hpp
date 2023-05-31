// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SEMSTREAM_HPP_
#define OPTFRAME_SEMSTREAM_HPP_

// This is a fundamental class for providing Semantic Streams on OptFrame
// The stream indicates its purpose, so formatting can change according to
// stream

#include <iostream>
#include <sstream>

namespace optframe {

// semantic streams (optframe::cjson, ctxt, etc)
struct SemStream : private std::streambuf, public std::ostream {
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

// extern SemStream cjson; // only in .cpp
// extern SemStream ctxt;  // only in .cpp
inline SemStream cjson{};  // (C++17 extern linkage is implicit)
inline SemStream ctxt{};   // (C++17 extern linkage is implicit)

}  // namespace optframe

#endif  // OPTFRAME_SEMSTREAM_HPP_
