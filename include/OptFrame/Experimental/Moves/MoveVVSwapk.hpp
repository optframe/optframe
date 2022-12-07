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

#ifndef OPTFRAME_MOVEVVSwapK_HPP_
#define OPTFRAME_MOVEVVSwapK_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../NSVector.hpp"

using namespace std;

//============================================================================
//                           VVSwapk Move
//============================================================================

template <class T, class DS>
class MoveVVSwapk : public Move<vector<vector<T>>, DS> {
 public:
  int k1, k2, v1, p1, v2, p2;

  MoveVVSwapk(int k1, int k2, int v1, int p1, int v2, int p2) {
    this->k1 = k1;
    this->k2 = k2;
    this->v1 = v1;
    this->p1 = p1;
    this->v2 = v2;
    this->p2 = p2;
  }

  virtual bool canBeApplied(const vector<vector<T>>&) { return true; }

  virtual Move<vector<vector<T>>, DS>& apply(vector<vector<T>>& rep) {
    pair<pair<int, int>, pair<pair<int, int>, pair<int, int>>> m;
    m.first.first = k1;
    m.first.second = k2;
    m.second.first.first = v1;
    m.second.first.second = p1;
    m.second.second.first = v2;
    m.second.second.second = p2;
    NSVector<int>::swapk_apply(rep, m);

    return *new MoveVVSwapk<T, DS>(k1, k2, v2, p2, v1, p1);
  }

  virtual Move<vector<vector<T>>, DS>& apply(DS& m, vector<vector<T>>& r) {
    if (!m.empty()) {
      m[v1].first = m[v2].first = -1;
      m[v1].second.first = p1;
      m[v1].second.second = r[v1].size() - 1;
      m[v2].second.first = p2;
      m[v2].second.second = r[v2].size() - 1;
    } else {
      // e->setMemory(new
      // MemVRP(r.size(),make_pair(-1,make_pair(0,r.size()-1))));
      m = MemVRPTW(r.size(), make_pair(-1, make_pair(0, r.size() - 1)));
    }

    return apply(r);
  }

  void print() const override {
    cout << "Move Vector Vector Swapk(" << k1 << " " << k2 << " " << v1 << " "
         << p1 << " " << v2 << " " << p2 << ")" << endl;
  }

  virtual bool operator==(const Move<vector<vector<T>>, DS>& m) const {
    return false;  // TODO
  }
};

#endif /*OPTFRAME_MOVEVVSwapK_HPP_*/
