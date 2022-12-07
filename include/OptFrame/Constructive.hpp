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

#ifndef OPTFRAME_CONSTRUCTIVE_HPP_
#define OPTFRAME_CONSTRUCTIVE_HPP_

#include <OptFrame/BaseConcepts.hpp>

#include "Component.hpp"
//#include "Solution.hpp"
//#include "Solutions/CopySolution.hpp"
//#include "InitialSearch.hpp" // TODO: remove

namespace optframe {

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R, ADS> S =
// CopySolution<R, ADS>>
template <XSolution S>
class Constructive : public Component {
 public:
  virtual ~Constructive() {}

  // timelimit in seconds, accepting fractions (millisecs, ...)
  // may or may not generate valid solution in time
  virtual std::optional<S> generateSolution(double timelimit) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    std::string s = Component::idComponent();
    ss << s << ":Constructive";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};
//
}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_HPP_*/
