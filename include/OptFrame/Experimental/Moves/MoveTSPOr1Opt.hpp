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

#ifndef OPTFRAME_MOVETSPOR1OPT_HPP_
#define OPTFRAME_MOVETSPOR1OPT_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template <class T, class DS>
class MoveTSPOr1Opt : public Move<vector<T>, DS> {
 private:
  int c;    // client
  int pos;  // position

  typedef vector<T> Route;

 public:
  MoveTSPOr1Opt(int _c, int _pos) : c(_c), pos(_pos) {}

  virtual ~MoveTSPOr1Opt() {}

  int getClient() { return c; }

  int getPosition() { return pos; }

  bool canBeApplied(const Route& rep) {
    bool all_positive = (c >= 0) && (pos >= 0);
    return all_positive && (c != pos) && (c + 1 != pos);
  }

  MoveTSPOr1Opt& apply(Route& rep) {
    T aux;
    if (c < pos) {
      for (int i = c; i < (pos - 1); i++) {
        aux = rep.at(i);
        rep.at(i) = rep.at(i + 1);
        rep.at(i + 1) = aux;
      }
      return *new MoveTSPOr1Opt(pos - 1, c);
    } else {
      for (int i = c; i > pos; i--) {
        aux = rep.at(i);
        rep.at(i) = rep.at(i - 1);
        rep.at(i - 1) = aux;
      }
      return *new MoveTSPOr1Opt(pos, c + 1);
    }

    return *new MoveTSPOr1Opt(-1, -1);
  }

  bool operator==(const Move<vector<T>, DS>& _m) const {
    const MoveTSPOr1Opt& m1 = (const MoveTSPOr1Opt&)_m;
    return (m1.c == c) && (m1.pos == pos);
  }

  void print() const override {
    std::cout << "MoveTSPOr1Opt";
    std::cout << "( " << c << " , " << pos << " )" << std::endl;
  }
};

#endif /*OPTFRAME_MOVETSPOR1OPT_HPP_*/
