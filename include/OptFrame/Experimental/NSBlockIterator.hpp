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

#ifndef OPTFRAME_EXPERIMENTAL_NSBLOCKITERATOR_HPP_
#define OPTFRAME_EXPERIMENTAL_NSBLOCKITERATOR_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <string>
#include <utility>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/NSIterator.hpp>

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

// NSBlockIterator: iterates from blocks (parts) of the neighborhood structure
// these parts may share any characteristic that may help predicting the
// behavior of local optima

MOD_EXPORT template <XSolution S, XEvaluation XEv = Evaluation<>,
                     XESolution XES = std::pair<S, XEv>>
class NSBlockIterator : public Component {
 public:
  virtual ~NSBlockIterator() {}

  virtual void first() = 0;
  virtual void next() = 0;
  virtual bool isDone() = 0;
  virtual NSIterator<XES>& current() = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":NSBlockIterator";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = std::pair<S, XEv>>
class DefaultNSBlockIterator : public NSBlockIterator<S, XEv, XES> {
 protected:
  NSIterator<XES>* it;

 public:
  DefaultNSBlockIterator(NSIterator<XES>& _it) : it(&_it) {}

  virtual ~DefaultNSBlockIterator() {}

  void first() override {}

  void next() override { it = nullptr; }

  bool isDone() override { return it == nullptr; }

  virtual NSIterator<XES>& current() override { return *it; }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":NSBlockIterator";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif  // OPTFRAME_EXPERIMENTAL_NSBLOCKITERATOR_HPP_
