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

#ifndef OPTFRAME_IEVALUATOR_HPP_
#define OPTFRAME_IEVALUATOR_HPP_

#include "BaseConcepts.hpp"

namespace optframe {

template <XESolution XES>
class IEvaluator  // NO INHERITANCE HERE!
{
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual XEv evaluate(const S&) = 0;

  // DO NOT CREATE id() OR ANY COMPONENT-like FEATURE
  // THIS JUST WORKS TO COORDINATE TWO CLASSES
  // - Evaluator
  // - MultiEvaluator
  // THIS COULD BE REMOVED IN THE FUTURE
};

}  // namespace optframe

#endif  // OPTFRAME_IEVALUATOR_HPP_
