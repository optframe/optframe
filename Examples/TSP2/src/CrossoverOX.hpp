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

#ifndef EXAMPLES_TSP2_SRC_CROSSOVEROX_HPP_
#define EXAMPLES_TSP2_SRC_CROSSOVEROX_HPP_

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Heuristics/EvolutionaryAlgorithms/Crossover.hpp"
#include "../../OptFrame/Solution.hpp"
#include "Representation.h"

using namespace optframe;

namespace TSP2 {

class CrossoverOX : public Crossover<RepTSP> {
 protected:
  RandGen& rg;

 public:
  CrossoverOX(RandGen& _rg)
      : rg(_rg) {
  }

  virtual ~CrossoverOX() {
  }

  virtual pair<Solution<RepTSP>*, Solution<RepTSP>*> cross(const Solution<RepTSP>& s1, const Solution<RepTSP>& s2) {
    pair<Solution<RepTSP>*, Solution<RepTSP>*> r(NULL, NULL);

    const vector<int>& r1 = s1.getR();
    const vector<int>& r2 = s2.getR();

    // impossible to cross!
    if ((r1.size() != r2.size()) || (r1.size() < 4))
      return r;

    Solution<RepTSP>& ch1 = s1.clone();
    Solution<RepTSP>& ch2 = s2.clone();

    int i = r1.size() - 1;
    while (i >= (int(r1.size()) - 1))
      i = rg.rand(r1.size());

    int j = i;
    while (j <= i)
      j = rg.rand(r1.size());

    // Okay! i < j and (j-i) >= 1 (size of at least 2)
    doCross(r1, r2, i, j, ch1.getR());
    doCross(r2, r1, i, j, ch2.getR());

    r.first = &ch1;
    r.second = &ch2;

    return r;
  }

  // TODO: COMPARE!!
  //void doCross(const vector<int>& A, const vector<int>& B, int i, int j, vector<int>& child)
  //vector<bool> vmap(max(vmax(A), vmax(B))+1, false);
  void doCross(const vector<int>& A, const vector<int>& B, unsigned i, unsigned j, vector<int>& child) {
    vector<int> middle(A.begin() + i, A.begin() + j);

    child = B;

    for (unsigned k = 0; k < child.size(); k++)
      if (in(child[k], middle))
        child[k] = -1;

    vector<int> list;
    for (unsigned k = i; k < child.size(); k++)
      if (child[k] != -1)
        list.push_back(child[k]);
    for (unsigned k = 0; k < i; k++)
      if (child[k] != -1)
        list.push_back(child[k]);

    child.clear();
    child.insert(child.end(), list.begin(), list.begin() + i);
    child.insert(child.end(), middle.begin(), middle.end());
    child.insert(child.end(), list.begin() + i, list.end());
  }

  bool in(int k, const vector<int>& v) {
    for (unsigned i = 0; i < v.size(); i++)
      if (v[i] == k)
        return true;
    return false;
  }

  int vmax(const vector<int>& v) {
    int m = v.at(0);
    for (unsigned i = 1; i < v.size(); i++)
      if (v[i] > m)
        m = v[i];
    return m;
  }

  void classicTest() {
    std::cout << "========== OX test begins ==========" << std::endl;
    vector<int> v1;
    v1.push_back(9);
    v1.push_back(8);
    v1.push_back(4);
    v1.push_back(5);
    v1.push_back(6);
    v1.push_back(7);
    v1.push_back(1);
    v1.push_back(3);
    v1.push_back(2);
    v1.push_back(0);
    vector<int> v2;
    v2.push_back(8);
    v2.push_back(7);
    v2.push_back(1);
    v2.push_back(2);
    v2.push_back(3);
    v2.push_back(0);
    v2.push_back(9);
    v2.push_back(5);
    v2.push_back(4);
    v2.push_back(6);

    vector<int> r;
    doCross(v1, v2, 3, 6, r);

    std::cout << "v1=" << v1 << std::endl;
    std::cout << "v2=" << v2 << std::endl;
    std::cout << "r=" << r << std::endl;
    std::cout << "========= OX test finished =========" << std::endl;
  }
};

}  // namespace TSP2

#endif  // EXAMPLES_TSP2_SRC_CROSSOVEROX_HPP_
