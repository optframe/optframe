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

#ifndef OPTFRAME_ISOLUTION_HPP_
#define OPTFRAME_ISOLUTION_HPP_

#include <cstdlib>
#include <iostream>

// C
#include <assert.h>
//
#include <OptFrame/BaseConcepts.hpp>  // concepts c++20
#include <OptFrame/IEvaluation.hpp>

namespace optframe {

// ISolution should compatible with XSolution space
class ISolution {
 public:
  virtual std::string toString() const = 0;

  virtual ~ISolution() {
  }
};

// IRSolution should compatible with XRSolution space
template <XRepresentation R>
class IRSolution : public ISolution {
 public:
  virtual R& getR() = 0;  // too bad, virtual cannot return deduced type XRepresentation (cannot remove template here)
  virtual std::string toString() const = 0;

  virtual ~IRSolution() {
  }
};

// IESolution should compatible with XESolution space (XSolution and XEvaluation)
// NOTE THAT: XSolutionOrIncomplete allows for ANY incomplete type
// and it may be XSolution or not... compiler caching may generate undefined behavior (beware!)
// We need this to allow easy CRTP over IESolution.
template <XSolutionOrIncomplete S, XEvaluation XEv>
class IESolution : public ISolution {
 public:
  //S first;    // non-static declared with placeholder XSolution (requires template here!)
  //XEv second; // non-static declared with placeholder XEvaluation (requires template here!)

  using first_type = S;
  using second_type = XEv;

  S& first;     // non-static declared with placeholder XSolution (requires template here!)
  XEv& second;  // non-static declared with placeholder XEvaluation (requires template here!)

  // give me the references of something YOU own, not me...
  explicit IESolution(S& s, XEv& e)
      : first(s), second(e) {
  }

  /*
   explicit IESolution(S&& s, XEv&& e) :
      first(s), second(e)
   {
   }
*/

  virtual ~IESolution() {
  }
};

}  // namespace optframe

#endif /* OPTFRAME_ISOLUTION_HPP_ */
