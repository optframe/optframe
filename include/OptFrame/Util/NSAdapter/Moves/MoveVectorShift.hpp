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

#ifndef OPTFRAME_MOVEVECTORSHIFT_HPP_
#define OPTFRAME_MOVEVECTORSHIFT_HPP_

// Framework includes
#include <OptFrame/Core/Move.hpp>

// using namespace std;
// using namespace optframe; (?????????????????) Don't use namespace
// declarations in headers

namespace optframe {

//============================================================================
//                           Shift Move
//============================================================================

template <class T>
class MoveVectorShift : public Move<vector<T>> {
 private:
  int t1, t2;
  int n;

 public:
  MoveVectorShift(int t1, int t2) {
    this->t1 = t1;
    this->t2 = t2;
  }

  bool canBeApplied(Solution<vector<T>>* s) { return t1 != t2; }

  Move<vector<T>>* apply(Solution<vector<T>>* s) {
    vector<T>* rep = s->getRepresentation();

    T aux;
    if (t1 < t2)
      for (int i = t1; i < t2; i++) {
        aux = rep->at(i);
        (*rep)[i] = (*rep)[i + 1];
        (*rep)[i + 1] = aux;
      }
    else
      for (int i = t1; i > t2; i--) {
        aux = rep->at(i);
        (*rep)[i] = (*rep)[i - 1];
        (*rep)[i - 1] = aux;
      }

    MoveVectorShift<T>* ms = new MoveVectorShift<T>(t2, t1);

    return ms;
  }

  void print() const override {
    std::cout << "Move Vector Shift(" << t1 << " -> " << t2 << ")" << std::endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOVEVECTORSHIFT_HPP_*/
