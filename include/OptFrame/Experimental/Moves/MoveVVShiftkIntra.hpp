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

#ifndef OPTFRAME_MOVEVVShiftkIntra_HPP_
#define OPTFRAME_MOVEVVShiftkIntra_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../NSVector.hpp"

using namespace std;

//============================================================================
//                           VVShiftkIntra Move
//============================================================================

template <class T, class DS>
class MoveVVShiftkIntra : public Move<vector<vector<T>>, DS> {
 public:
  int k, v, p1, p2;

  MoveVVShiftkIntra(int k, int v, int p1, int p2) {
    this->k = k;
    this->v = v;
    this->p1 = p1;
    this->p2 = p2;
  }

  virtual bool canBeApplied(const vector<vector<T>>&) { return true; }

  virtual Move<vector<vector<T>>, DS>& apply(vector<vector<T>>& rep) {
    pair<int, pair<int, int>> m;
    m.first = k;
    m.second.first = p1;
    m.second.second = p2;
    NSVector<T>::shiftk_apply(rep[v], m);

    return *new MoveVVShiftkIntra<T, DS>(k, v, p2, p1);
  }

  virtual Move<vector<vector<T>>, DS>& apply(DS& m, vector<vector<T>>& r) {
    if (!m.empty()) {
      m[v].first = -1;
      m[v].second.first = p1;
      m[v].second.second = r[v].size() - 1;
    } else {
      // e->setMemory(new
      // MemVRP(r.size(),make_pair(-1,make_pair(0,r.size()-1))));
      m = MemVRPTW(r.size(), make_pair(-1, make_pair(0, r.size() - 1)));
    }

    return apply(r);
  }

  void print() const override {
    cout << "Move VRP ShiftkIntra(" << k << " " << v << " " << p1 << " " << p2
         << ")" << endl;
  }

  virtual bool operator==(const Move<vector<vector<T>>, DS>& m) const {
    return false;  // TODO
  }
};

#endif /*OPTFRAME_MOVEVVShiftkIntra_HPP_*/
