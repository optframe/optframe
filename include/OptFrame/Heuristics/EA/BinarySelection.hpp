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

#ifndef BINARY_SELECTION_HPP_
#define BINARY_SELECTION_HPP_

#include <assert.h>

#include <vector>

#include "../../MultiEvaluation.hpp"
#include "../../MultiSolution.hpp"
#include "../../RandGen.hpp"
#include "Selection.hpp"

namespace optframe {

template <XSolution S>
class BinarySelection : public Selection<R, ADS> {
 private:
  RandGen& rg;

 public:
  BinarySelection(RandGen& _rg)
      : rg(_rg) {
  }

  virtual ~BinarySelection() {
  }

  pair<unsigned, unsigned> select(const MultiSolution<S>& p, const MultiEvaluation<>& mev, const vector<double>& fv) {
    assert(p.size() > 1);
    //cout << "SELECT: " << p.size() << endl;
    //cout << fv << " = " << Selection<R, ADS>::getSum(fv) << endl;

    unsigned s1 = 0;

    float x = rg.rand01();
    //cout << "x=" << x << endl;
    for (unsigned i = 0; i < fv.size(); i++) {
      x -= fv[i];
      if (x <= 0) {
        s1 = i;
        break;
      }
    }

    unsigned s2 = s1;
    //cout << "s1 = " << s1 << endl;
    int trye = 0;
    while (s2 == s1) {
      trye++;
      if (trye >= 100 * p.size()) {
        cout << "LOOP IN BINARYSELECTION??" << endl;
        cout << fv << " = " << Selection<R, ADS>::getSum(fv) << endl;
        exit(1);
      }
      x = rg.rand01();
      //cout << "x2=" << x << endl;
      for (unsigned i = 0; i < fv.size(); i++) {
        x -= fv[i];
        if (x <= 0) {
          s2 = i;
          break;
        }
      }
    }

    return make_pair(s1, s2);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Selection<R, ADS>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Selection<R, ADS>::idComponent() << ":BinarySelection";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /* BINARY_SELECTION_HPP_ */
