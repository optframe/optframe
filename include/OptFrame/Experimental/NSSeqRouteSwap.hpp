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

#ifndef OPTFRAME_NSSEQROUTESWAP_HPP_
#define OPTFRAME_NSSEQROUTESWAP_HPP_

#include <vector>

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template <class T, class DS>
class MoveVVSwapIntra : public Move<vector<vector<T>>, DS> {
 private:
  int i, j, k;

 public:
  MoveVVSwapIntra(int i, int j, int k) {
    this->i = i;
    this->j = j;
    this->k = k;
  }

  bool canBeApplied(std::vector<vector<T>>* rep) {
    return (rep->size() > 0) && (j != k);
  }

  Move<vector<vector<T>>, DS>* apply(std::vector<vector<T>>* rep) {
    T aux = (*rep)[i][j];
    (*rep)[i][j] = (*rep)[i][k];
    (*rep)[i][k] = aux;

    return new MoveVVSwapIntra(i, k, j);
  }

  bool equals(Move<vector<vector<T>>, DS>* m) {
    MoveVVSwapIntra* m1 = (MoveVVSwapIntra*)m;

    return (m1->i == i) &&
           ((m1->j == j && m1->k == k) || (m1->j == k && m1->k == j));
  }

  void print() const override {
    std::cout << "MoveVVSwapIntra( " << i << " , ( " << j << " , " << k << " ) )"
         << std::endl;
  }
};

template <class T, class DS>
class NSSeqVVSwapIntra : public NSSeq<vector<vector<T>>, DS> {
 private:
  int m_i, m_j, m_k;

 public:
  NSSeqVVSwapIntra() {}

  Move<vector<vector<T>>, DS>* move(std::vector<vector<T>>* rep) {
    if (rep->size() == 0) return new MoveVVSwapIntra<T, DS>(0, 0, 0);

    int i = rand() % rep->size();

    int n = rep->at(i).size();

    if (n == 0) return new MoveVVSwapIntra<T, DS>(i, 0, 0);

    int j = rand() % n;
    int k = j;

    if (n > 1)
      while (k == j) k = rand() % n;

    return new MoveVVSwapIntra<T, DS>(i, j, k);
  }

  void init(std::vector<vector<T>>* rep)  // Initialize neighborhood structure
  {
    m_i = 0;
    m_j = 0;
    m_k = 0;
  }

  Move<vector<vector<T>>, DS>* next(std::vector<vector<T>>* rep) {
    if (m_i >= rep->size()) {
      std::cout << "Erro!!" << std::endl;
      exit(1);
    }

    int n2 = rep->at(m_i).size();

    if (m_k < n2 - 1)
      m_k++;
    else if (++m_j < n2 - 1) {
      m_k = m_j + 1;
    } else {
      m_i++;
      m_j = 0;
      m_k = 0;
    }

    return new MoveVVSwapIntra<T, DS>(m_i, m_j, m_k);
  }

  bool hasNext(std::vector<vector<T>>* rep) {
    int n1 = rep->size();
    int n2 = rep->at(m_i).size();

    if ((m_i >= n1 - 1) && (m_j >= n2 - 2) && (m_k >= n2 - 1))
      return false;
    else
      return true;
  }

  virtual void print() { std::cout << "NSSeqVVSwapIntra" << std::endl; }
};

#endif /*OPTFRAME_NSSEQROUTESWAP_HPP_*/
