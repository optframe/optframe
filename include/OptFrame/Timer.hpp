// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef TIMER_HPP
#define TIMER_HPP

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C includes
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT class Timer : public Component {
 private:
  bool showMessageOnDestroy;

#ifdef WIN32
  LARGE_INTEGER frequency;
  LARGE_INTEGER tstart;
#else
  struct timeval tstart;
#endif

 public:
  Timer(bool m = false) : showMessageOnDestroy(m) {
#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&tstart);
#else
    struct timezone tz;
    gettimeofday(&tstart, &tz);
#endif
  }

  virtual ~Timer() {
    if (showMessageOnDestroy) std::printf("Spent time: %f secs\n", now());
  }

  double now() const { return inSecs(); }

  double inSecs() const { return inMicroSecs() * 0.000001; }

  double inMilliSecs() const { return inMicroSecs() * 0.001; }

  double inMicroSecs() const {
    double start;
    double end;

#ifdef WIN32
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);

    start = tstart.QuadPart * (1000000.0 / frequency.QuadPart);
    end = t.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    struct timeval t;
    struct timezone tz;
    gettimeofday(&t, &tz);

    start = (tstart.tv_sec * 1000000.0) + tstart.tv_usec;
    end = (t.tv_sec * 1000000.0) + t.tv_usec;
#endif

    return end - start;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":Timer";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }
};

}  // namespace optframe

#endif /* TIMER_HPP */
