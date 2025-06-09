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

#ifndef OPTFRAME_HELPER_INITIALMULTISOLUTION_HPP_
#define OPTFRAME_HELPER_INITIALMULTISOLUTION_HPP_

// C++
#include <string>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/MultiSolution.hpp>

// #include "FamilyEA.hpp"

// using namespace std;

namespace optframe {

template <XSolution S, X2Solution<S> X2S = MultiSolution<S>>
class InitialMultiSolution : public Component {
 public:
  virtual ~InitialMultiSolution() {}

  virtual X2S generatePopulation(unsigned populationSize) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":InitialMultiSolution";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// ==========================================================
// BasicInitialMultiSolution<S> -> generates MultiSolution<S>
// ==========================================================

template <XSolution S>
class BasicInitialMultiSolution
    : public InitialMultiSolution<S, MultiSolution<S>> {
 public:
  sref<Constructive<S>> constructive;

  explicit BasicInitialMultiSolution(sref<Constructive<S>> _constructive)
      : constructive(_constructive) {}

  virtual ~BasicInitialMultiSolution() {}

  MultiSolution<S> generatePopulation(unsigned populationSize) override {
    MultiSolution<S> p;
    // TODO(igormcoelho): generatePopulation must have timelimit!
    double timelimit = 0.0;
    for (unsigned i = 0; i < populationSize; i++)
      p.push_back(*constructive->generateSolution(timelimit));
    return p;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (InitialMultiSolution<S>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialMultiSolution<S>::idComponent() << ":BasicInitialPopulation";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HELPER_INITIALMULTISOLUTION_HPP_
