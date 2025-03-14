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

#ifndef OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_
#define OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Helper/Solution.hpp>

#include "GRASPFamily.h"

namespace optframe {

// Greedy Randomized Constructive
template <XSolution S>
class GRConstructive : public Constructive<S>, public GRASP {
 public:
  virtual ~GRConstructive() {}

  virtual std::optional<S> generateGRSolution(float alpha,
                                              double timelimit) = 0;

  virtual std::optional<S> generateSolution(double timelimit) override {
    return generateGRSolution(1.0, timelimit);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << GRASP::family()
       << ":GRConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// BasicGRConstructive can envelop a Constructive
template <XSolution S>
class BasicGRConstructive : public GRConstructive<S> {
 public:
  Constructive<S>& c;

  BasicGRConstructive(Constructive<S>& _c) : c(_c) {}

  virtual ~BasicGRConstructive() {}

  virtual op<S> generateGRSolution(float alpha, double timelimit) override {
    // ignoring alpha
    return c.generateSolution(timelimit);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << GRASP::family()
       << ":GRConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif /*OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_*/
