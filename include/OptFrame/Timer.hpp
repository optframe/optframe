// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef TIMER_HPP
#define TIMER_HPP

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

namespace optframe {

class Timer : public Component {
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
    if (showMessageOnDestroy) printf("Spent time: %f secs\n", now());
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

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":Timer";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class TimerBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~TimerBuilder() {}

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    return new Timer;
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    return params;
  }

  bool canBuild(std::string component) override {
    return component == Timer::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "Timer";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /* TIMER_HPP */
