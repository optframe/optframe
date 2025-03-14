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

#ifndef OPTFRAME_CROSSOVER_PMX_HPP_
#define OPTFRAME_CROSSOVER_PMX_HPP_

#include "../../Evaluation.hpp"
#include "../../Helper/Solution.hpp"
#include "GeneralCrossover.hpp"

namespace optframe {

template <class ADS = OPTFRAME_DEFAULT_ADS>
class CrossoverPMX : public GeneralCrossover<vector<int>, ADS> {
 protected:
  RandGen& rg;

 public:
  CrossoverPMX(RandGen& _rg)
      : rg(_rg) {
  }

  virtual ~CrossoverPMX() {
  }

  virtual pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> cross(const Solution<vector<int>, ADS>& s1, const Solution<vector<int>, ADS>& s2) {
    pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> r(NULL, NULL);

    const vector<int>& r1 = s1.getR();
    const vector<int>& r2 = s2.getR();

    // impossible to cross!
    if ((r1.size() != r2.size()) || (r1.size() < 3))
      return r;

    Solution<vector<int>, ADS>& ch1 = s1.clone();
    Solution<vector<int>, ADS>& ch2 = s2.clone();

    int i = r1.size() - 1;
    int j = i;
    while ((j <= i) || (j - i) == (r1.size() - 1))
      j = rg.rand(r1.size() + 1);  // including end of vector

    // Okay! i < j and (j-i) >= 1 (size of at least 2)
    ch1.getR() = doCross(r1, r2, i, j);
    ch2.getR() = doCross(r2, r1, i, j);

    r.first = &ch1;
    r.second = &ch2;

    return r;
  }

  vector<int> doCross(const vector<int>& A, const vector<int>& B, unsigned i, unsigned j) {
    vector<int> middle(A.begin() + i, A.begin() + j);
    vector<int> middleB(B.begin() + i, B.begin() + j);

    //cout << "MIDDLE=" << middle << std::endl;
    //cout << "MIDDLEB=" << middleB << std::endl;

    // mark repeated elements with -1
    for (unsigned k = 0; k < middle.size(); k++)
      for (unsigned z = 0; z < middleB.size(); z++)
        if (middle[k] == middleB[z]) {
          middle[k] = -1;
          middleB[z] = -1;
          break;  // break first loop
        }

    // create a map of updates
    map<int, int> vmap_a;
    unsigned b1 = 0;
    for (unsigned k = 0; k < middle.size(); k++)
      if (middle[k] != -1)
        for (unsigned z = b1; z < middleB.size(); z++)
          if (middleB[z] != -1) {
            vmap_a[middle[k]] = middleB[z];
            b1 = z + 1;
            break;
          }

    // create child with duplications
    vector<int> child(B);
    for (unsigned k = i; k < j; k++)
      child[k] = A[k];

    //cout << "Bug child (B+middle)=" << child << std::endl;

    // fix using the map
    for (int k = ((int)child.size()) - 1; k >= 0; k--) {
      map<int, int>::iterator it = vmap_a.find(child[k]);
      if (it != vmap_a.end()) {
        child[k] = it->second;
        vmap_a.erase(it);
      }
    }

    return child;
  }

  void classicTest() {
    std::cout << "========== PMX test begins ==========" << std::endl;
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

    vector<int> r = doCross(v1, v2, 3, 8);

    std::cout << "v1=" << v1 << std::endl;
    std::cout << "v2=" << v2 << std::endl;
    std::cout << "r=" << r << std::endl;

    vector<int> r2 = doCross(v2, v1, 3, 8);
    std::cout << "r2=" << r2 << std::endl;

    std::cout << "========= PMX test finished =========" << std::endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_CROSSOVER_PMX_HPP_*/
