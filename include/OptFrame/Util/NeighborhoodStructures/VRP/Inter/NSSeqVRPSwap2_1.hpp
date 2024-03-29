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

#ifndef OPTFRAME_NSSEQVRPSWAP2_1_HPP_
#define OPTFRAME_NSSEQVRPSWAP2_1_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPSwap2_1 : public Move<vector<vector<T>>, ADS, DS> {
  typedef vector<vector<T>> Routes;

 protected:
  int r1, r2;    // route 1 and route 2;
  int c1, c2;    // position 1 and position 2 of the clients, respectively
  bool reverse;  // true -> reverse moviment

 public:
  MoveVRPSwap2_1(int _r1, int _r2, int _c1, int _c2, bool _reverse,
                 OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : r1(_r1),
        r2(_r2),
        c1(_c1),
        c2(_c2),
        reverse(_reverse),
        problem(_problem) {}

  virtual ~MoveVRPSwap2_1() {}

  bool get_reverse() { return reverse; }

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_c1() { return c1; }

  int get_c2() { return c2; }

  virtual bool canBeApplied(const Routes& rep, const ADS*) override {
    bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 0) && (c2 >= 0);
    return all_positive && (rep.size() >= 2) && (rep.at(r1).size() >= 2) &&
           (rep.at(r2).size() > 0);
  }

  virtual Move<Routes, ADS, DS>* apply(Routes& rep, ADS*) override {
    int aux;

    if (reverse) {
      aux = rep.at(r1).at(c1);
      rep.at(r1).at(c1) = rep.at(r1).at(c1 + 1);
      rep.at(r1).at(c1 + 1) = aux;
    }

    // swapping
    aux = rep.at(r1).at(c1);
    rep.at(r1).at(c1) = rep.at(r2).at(c2);
    rep.at(r2).at(c2) = aux;

    rep.at(r2).insert(rep.at(r2).begin() + c2 + 1, rep.at(r1).at(c1 + 1));
    rep.at(r1).erase(rep.at(r1).begin() + c1 + 1);

    // p->p, r1->r2, r2->r1, c1->c2, c2->c1, reverse->reverse,
    return new MoveVRPSwap2_1<T, ADS, DS>(r2, r1, c2, c1, reverse);
  }

  virtual bool operator==(const Move<Routes>& _m) const {
    const MoveVRPSwap2_1<T, ADS, DS>& m1 =
        (const MoveVRPSwap2_1<T, ADS, DS>&)_m;
    return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2) &&
            (m1.reverse == reverse));
  }

  void print() const override {
    cout << "MoveVRPSwap2_1( ";
    cout << r1 << ", " << r2 << ", " << c1 << ", " << c2 << ", " << reverse
         << " )";
    cout << endl;
  }
};

template <
    class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
    class MOVE = MoveVRPSwap2_1<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPSwap2_1 : public NSIterator<vector<vector<T>>> {
  typedef vector<vector<T>> Routes;

 private:
  MOVE* m;
  vector<uptr<Move<XES>>> moves;
  int index;  // index of moves
  const Routes& r;

  P* p;  // has to be the last
 public:
  NSIteratorVRPSwap2_1(const Routes& _r) : r(_r), p(_p) {
    m = nullptr;
    index = 0;
  }

  virtual ~NSIteratorVRPSwap2_1() {
    /*for (int i = 0; i < moves.size(); i++)
               delete moves[i];*/
  }

  virtual void first() override {
    for (int r1 = 0; r1 < r.size(); r1++) {
      if (r.at(r1).size() >= 2) {
        for (int r2 = 0; r2 < r.size(); r2++) {
          if (r1 != r2) {
            for (int c1 = 0; c1 < ((int)r.at(r1).size()) - 1; c1++) {
              for (int c2 = 0; c2 < r.at(r2).size(); c2++) {
                moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                    new MOVE(r1, r2, c1, c2, false, p)));  // normal
                moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                    new MOVE(r1, r2, c1, c2, true, p)));  // reverse
              }
            }
          }
        }
      }
    }
    if (moves.size() > 0) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual void next() override {
    index++;
    if (index < moves.size()) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual bool isDone() override { return m == nullptr; }

  virtual Move<T, ADS, DS>* current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPSwap2_1. Aborting." << endl;
      exit(1);
    }

    return m;
  }
};

template <
    class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
    class MOVE = MoveVRPSwap2_1<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM,
    class NSITERATOR = NSIteratorVRPSwap2_1<T, ADS, DS, MOVE, P>>
class NSSeqVRPSwap2_1 : public NSSeq<vector<vector<T>>> {
  typedef vector<vector<T>> Routes;

 private:
  P* p;  // has to be the last

 public:
  NSSeqVRPSwap2_1(P* _p = nullptr) : p(_p) {}

  virtual ~NSSeqVRPSwap2_1() {}

  uptr<Move<Routes, ADS, DS>> randomMove(const Routes& rep,
                                         const ADS*) override {
    if (rep.size() < 2)
      return uptr<Move<Routes, ADS, DS>>(MOVE(-1, -1, -1, -1, false, p));

    int r1 = rand() % rep.size();

    if (rep.at(r1).size() < 2)
      return uptr<Move<Routes, ADS, DS>>(MOVE(-1, -1, -1, -1, false, p));

    int r2;

    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    if (rep.at(r2).size() < 1)
      return uptr<Move<Routes, ADS, DS>>(MOVE(-1, -1, -1, -1, false, p));

    int c1 = rand() % (rep.at(r1).size() - 1);
    int c2 = rand() % rep.at(r2).size();

    bool reverse = rand() % 2;

    return uptr<Move<Routes, ADS, DS>>(MOVE(r1, r2, c1, c2, reverse, p));
  }

  virtual NSIteratorVRPSwap2_1<T, ADS, DS, MOVE, P>* getIterator(
      const Routes& r, const ADS*) override {
    return new NSITERATOR(r, p);
  }

  virtual void print() {
    cout << "NSSeqVRPSwap2_1 with move: " << MOVE::idComponent();
  }
};

#endif /*OPTFRAME_NSSEQVRPSWAP2_1_HPP_*/
