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

#ifndef OPTFRAME_IEVALUATION_HPP_
#define OPTFRAME_IEVALUATION_HPP_

// C
#include <assert.h>
// C++
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/SingleObjValue.hpp>
// #include "MultiObjValue.hpp"   // inserting this beforehand.. who knows!!!
// #include "SingleObjValue.hpp"  // basic value 'evtype' comes from here!

// using namespace std;

namespace optframe {

template <optframe::basic_arithmetics ObjType = evtype>
class IEvaluation {
 public:
  // too bad, cannot just put deduced type here (virtual function)
  virtual ObjType evaluation() const = 0;

  virtual std::string toString() const = 0;
};

}  // namespace optframe

#endif  // OPTFRAME_IEVALUATION_HPP_
