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

#ifndef TSP_NSENUMSwap_HPP_
#define TSP_NSENUMSwap_HPP_

// C++
#include <string>
// Framework includes
#include <OptFrame/Move.hpp>
#include <OptFrame/NSEnum.hpp>

// Own includes
#include <OptFrame/RandGen.hpp>

#include "ProblemInstance.h"
#include "Solution.h"

//#define MOV_Swap_DEBUG

using namespace std;

//============================================================================
//                           Swap MOVE
//============================================================================

namespace TSP {

class MoveSwap : public Move<ESolutionTSP> {
 private:
  int c1, c2;
  sref<ProblemInstance> tsp;

 public:
  MoveSwap(int c1, int c2, sref<ProblemInstance> _tsp) : tsp(_tsp) {
    this->c1 = c1;
    this->c2 = c2;

    // Put the rest of your code here
  }

  bool canBeApplied(const ESolutionTSP& s) override {
    // If there are some move "MoveSwap" that can't be applied, implement this
    // method
    //

    // only allows lower half quadratic moves
    // return (c1 < c2) && (c1 != 0);
    return true;
  }

  bool independentOf(const Move<ESolutionTSP>& _m) override {
    auto& m = (MoveSwap&)_m;

    assert(c1 < c2);
    assert(m.c1 < m.c2);

    // (0,1) nao existe!! pq são adjacentes!         0  1* 2 3* 4 5 6* 7 8* 9 0*
    // 1 2* 3 4 5 6 7* 8 9* -> 0 (c1,c2) -> (1, 3)     (6,8) são independentes?

    bool conflicts = (::abs(c1 - m.c1) <= 2) || (::abs(c1 - m.c2) <= 2) ||
                     (::abs(c2 - m.c1) <= 2) || (::abs(c2 - m.c2) <= 2);

    conflicts = conflicts || ((c1 == 0) && (c2 == tsp->n - 1)) ||
                ((c1 == 0) && (c2 == tsp->n - 2));

    return !conflicts;
  }

  uptr<Move<ESolutionTSP>> apply(ESolutionTSP& s) override {
    RepTSP& rep = s.first.getR();
    // Specify how the move "MoveSwap" will be applied

    int aux = rep.at(c1);
    rep[c1] = rep[c2];
    rep[c2] = aux;

    // return the reverse move
    return uptr<Move<ESolutionTSP>>(new MoveSwap(c2, c1, tsp));
  }

  uptr<Move<ESolutionTSP>> applyUpdate(ESolutionTSP& se) override {
    SolutionTSP& s = se.first;
    Evaluation<>& e = se.second;
    RepTSP& rep = s.getR();
    int k1, k2;

    if (c1 < c2) {
      k1 = c1;
      k2 = c2;
    } else {
      k1 = c2;
      k2 = c1;
    }

    // before k2 and k1
    int bk1 = k1 - 1;
    int bk2 = k2 - 1;
    // after k2 and k1
    int ak1 = k1 + 1;
    int ak2 = k2 + 1;

    if (k1 == 0) bk1 = rep.size() - 1;
    if (k2 == ((int)rep.size()) - 1) ak2 = 0;

    double f = 0;

    if (k2 - k1 == 1)  // special case, cities are near
    {
      f -= (*tsp->dist)(rep[bk1], rep[k1]);
      f -= (*tsp->dist)(rep[k1], rep[k2]);
      f -= (*tsp->dist)(rep[k2], rep[ak2]);
    } else {
      f -= (*tsp->dist)(rep[bk1], rep[k1]);
      f -= (*tsp->dist)(rep[k1], rep[ak1]);
      f -= (*tsp->dist)(rep[bk2], rep[k2]);
      f -= (*tsp->dist)(rep[k2], rep[ak2]);
    }

    uptr<Move<ESolutionTSP>> rev = apply(se);

    if (k2 - k1 == 1)  // special case, cities are near
    {
      f += (*tsp->dist)(rep[bk1], rep[k1]);
      f += (*tsp->dist)(rep[k1], rep[k2]);
      f += (*tsp->dist)(rep[k2], rep[ak2]);
    } else {
      f += (*tsp->dist)(rep[bk1], rep[k1]);
      f += (*tsp->dist)(rep[k1], rep[ak1]);
      f += (*tsp->dist)(rep[bk2], rep[k2]);
      f += (*tsp->dist)(rep[k2], rep[ak2]);
    }

    e.setObjFunction(e.getObjFunction() + f);
    // e.outdated = false;

    return rev;
  }

  op<EvaluationTSP> cost(const pair<SolutionTSP, Evaluation<>>& se,
                         bool allowEstimated) override {
    const SolutionTSP& s = se.first;
    const RepTSP& rep = s.getR();
    int k1, k2;

    if (c1 < c2) {
      k1 = c1;
      k2 = c2;
    } else {
      k1 = c2;
      k2 = c1;
    }

    // cout << "Swap k1=" << k1 << "(" << rep[k1] << ")" << " k2=" << k2 << "("
    // << rep[k2] << ")" << endl;

    // before k2 and k1
    int bk1 = k1 - 1;
    int bk2 = k2 - 1;
    // after k2 and k1
    int ak1 = k1 + 1;
    int ak2 = k2 + 1;

    if (k1 == 0) bk1 = ((int)rep.size()) - 1;
    if (k2 == ((int)rep.size()) - 1) ak2 = 0;

    double f = 0;

    if (k2 - k1 == 1)  // special case, cities are near (in fact, a 3-opt
                       // case... TODO remove this)
    {
      f -= (*tsp->dist)(rep[bk1], rep[k1]);
      f -= (*tsp->dist)(rep[k1], rep[k2]);
      f -= (*tsp->dist)(rep[k2], rep[ak2]);

      f += (*tsp->dist)(rep[bk1], rep[k2]);
      f += (*tsp->dist)(rep[k2], rep[k1]);
      f += (*tsp->dist)(rep[k1], rep[ak2]);
    } else if ((k1 == 0) &&
               (k2 == ((int)rep.size()) - 1))  // special case, extreme points
    {
      f -= (*tsp->dist)(rep[bk2], rep[k2]);
      f -= (*tsp->dist)(rep[k2], rep[k1]);
      f -= (*tsp->dist)(rep[k1], rep[ak1]);

      f += (*tsp->dist)(rep[bk2], rep[k1]);
      f += (*tsp->dist)(rep[k1], rep[k2]);
      f += (*tsp->dist)(rep[k2], rep[ak1]);
    } else {
      f -= (*tsp->dist)(rep[bk1], rep[k1]);
      f -= (*tsp->dist)(rep[k1], rep[ak1]);
      f -= (*tsp->dist)(rep[bk2], rep[k2]);
      f -= (*tsp->dist)(rep[k2], rep[ak2]);

      f += (*tsp->dist)(rep[bk1], rep[k2]);
      f += (*tsp->dist)(rep[k2], rep[ak1]);
      f += (*tsp->dist)(rep[bk2], rep[k1]);
      f += (*tsp->dist)(rep[k1], rep[ak2]);
    }

    // return new MoveCost<>(f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  /*
   void print() const override
   {
      cout << "MoveSwap between " << c1 << " and " << c2 << endl;
   }
*/
  std::string toString() const override {
    std::stringstream ss;
    ss << "MoveSwap(" << c1 << ";" << c2 << ")";
    return ss.str();
  }

  bool operator==(const Move<ESolutionTSP>& _m) const override {
    const MoveSwap& m =
        (const MoveSwap&)_m;  // You can only compare if types are equal

    if ((c1 == m.c1 && c2 == m.c2) || (c1 == m.c2 && c2 == m.c1))
      return true;
    else
      return false;
  }

#ifdef OPTFRAME_AC
  std::shared_ptr<Component> sharedClone() const override {
    std::shared_ptr<Component> sptr{new MoveSwap(c1, c2, tsp)};
    sptr->listAC = this->listAC;
    return sptr;
  }
#endif

  bool toStream(std::ostream& os) const override {
    // forward to operator<<
    os << (*this);
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const MoveSwap& mv) {
    if (&os == &optframe::cjson) {
      os << "{";
      os << "\"move_type\": \"MoveSwap\""
         << ",";
      os << "\"params\": [" << mv.c1 << ", " << mv.c2 << "]";
      os << "}";
    } else
      os << mv.toString();
    return os;
  }
};

//============================================================================
//                  Swap Neighborhood Structure
//============================================================================

class NSEnumSwap : public NSEnum<ESolutionTSP, ESolutionTSP> {
 private:
  sref<ProblemInstance> pI;
  int n;

  // Your private vars

 public:
  NSEnumSwap(sref<ProblemInstance> pI, sref<RandGen> _rg)
      : NSEnum<ESolutionTSP, ESolutionTSP>(_rg), pI{pI}, n{pI->n} {}

  // this NS type supports Move Independence
  bool supportsMoveIndependence() const override { return true; }

  // given index, returns (i,j), with 0 < i < j < n-1
  virtual uptr<Move<ESolutionTSP>> indexMove(unsigned int k) override {
    int i = k / (n - 1);
    int j = k % (n - 1) + 1;

    // last special detail...
    if (i >= j) {
      i = (n - 1) - i;
      j = (n - 1) - j + 1;
    }

    return uptr<Move<ESolutionTSP>>(new MoveSwap(i, j, pI));

    // Please, keep 'busca' for historical (and emotional) purposes :)
    // This was created in the night before the TCC presentation of OptFrame (in
    // 2009) And now, in 2017, a beautiful calculation is presented.
    // return busca(k, 1, 2 * n);
  }

  unsigned int size() const override { return n * (n - 1) / 2; }

  void print() const override { cout << "NSEnum Swap (" << size() << ")\n"; }

  // Auxiliar methods

  int corresp(int d) { return d - ((d - (n - 1)) - 1) * 2; }

  int numElem(int d) {
    if (d <= n)
      return (d / 2);
    else
      return numElem(corresp(d));
  }

  int comeca(int d) {
    if (d <= n) {
      int z = (d / 2);

      int ant = z * (z - 1);

      // Se impar, soma mais 'z'
      if (d % 2 == 1) ant += z;

      return ant;
    } else {
      return 2 * (comeca(n)) - comeca(corresp(d) + 1) + numElem(n);
    }
  }

  int termina(int d) { return comeca(d) + numElem(d) - 1; }

  Move<ESolutionTSP>& busca(int k, int a, int b) {
    int d = (a + b) / 2;

    // cout << "busca "<<k<<" na diagonal "<<d<<"entre ["<<a<<","<<b<<"]"<<endl;

    int c = comeca(d);
    int t = termina(d);

    // cout <<"comeca em "<<c<<" e termina em "<<t<<endl;

    // int p;
    // cin >>p;

    if (k < c) {
      // cout << "k<c"<<endl;
      return busca(k, a, d);
    }

    if (k > t) {
      // cout << "k>t"<<endl;

      if (a == d) d++;

      return busca(k, d, b);
    }

    if (d <= n) {
      for (int i = 0; i < numElem(d); i++)
        if (k == c + i) return *new MoveSwap(i, d - i - 1, *pI);
    } else {
      for (int i = 0; i < numElem(d); i++)
        if (k == c + i) {
          int j = d - n;
          return *new MoveSwap(i + j, d - i - 1 - j, *pI);
        }
    }

    cout << "Error!" << endl;
    return *new MoveSwap(0, 0, *pI);
  }
};

}  // end namespace TSP

#endif /*TSP_NSENUMSwap_HPP_*/
