// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRPEXCHANGE_HPP_
#define OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRPEXCHANGE_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

namespace optframe {

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPExchange : public Move<XES> {
  using XEv = typename XES::second_type;
  using Routes = std::vector<std::vector<int>>;

 public:
  Routes& (*getRoutes)(const XES&);  // function to get routes from type 'R'

 protected:
  int r;       // route
  int c1, c2;  // client 1 and client 2, respectively

 private:
  P* problem;

 public:
  MoveVRPExchange(Routes& (*_getRoutes)(const XES&), int _r, int _c1, int _c2,
                  P* _problem = nullptr)
      : getRoutes(_getRoutes), r(_r), c1(_c1), c2(_c2), problem(_problem) {}

  int get_c1() { return c1; }

  int get_c2() { return c2; }

  int get_r() { return r; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);  // se.first.getR();
    bool all_positive = (c1 >= 0) && (c2 >= 0) && (r >= 0);
    return all_positive && (rep.at(r).size() >= 2);
  }

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);  // se.first.getR();
    int aux = rep.at(r).at(c1);
    rep.at(r).at(c1) = rep.at(r).at(c2);
    rep.at(r).at(c2) = aux;

    return uptr<Move<XES>>(new MoveVRPExchange(getRoutes, r, c1, c2));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPExchange& m1 = (const MoveVRPExchange&)_m;
    return ((m1.c1 == c1) && (m1.c2 == c2) && (m1.r == r));
  }

  void print() const override {
    cout << "MoveVRPExchange( route: " << r << " : ";
    cout << " client " << c1 << " <=>  client" << c2 << " )";
    cout << endl;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRPExchange<XES, P>>
class NSIteratorVRPExchange : public NSIterator<XES> {
  typedef vector<vector<int>> Routes;

 protected:
  uptr<Move<XES>> m;
  int index;
  vector<uptr<Move<XES>>> moves;
  Routes& (*getRoutes)(const XES&);  // function to get routes from type 'R'
  const Routes& rep;
  P* p;  // has to be the last

 public:
  NSIteratorVRPExchange(Routes& (*getRoutes)(const XES&), const XES& se,
                        P* _p = nullptr)
      : m{nullptr}, index{0}, getRoutes(getRoutes), rep{getRoutes(se)}, p(_p) {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++) {
      for (int c1 = 0; c1 < (int)rep.at(r).size(); c1++) {
        for (int c2 = 0; c2 < (int)rep.at(r).size(); c2++) {
          if (c1 != c2)
            moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, c1, c2, p)));
        }
      }
    }
    if (moves.size() > 0)
      m = std::move(moves[index]);
    else
      m = nullptr;
  }

  void next() override {
    index++;
    if (index < (int)moves.size())
      m = std::move(moves[index]);
    else
      m = nullptr;
  }

  bool isDone() override { return m == nullptr; }

  uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPExchange. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRPExchange<XES, P>,
          class NSITERATOR = NSIteratorVRPExchange<XES, P, MOVE>>
class NSSeqVRPExchange : public NSSeq<XES> {
  typedef vector<vector<int>> Routes;

 public:
  Routes& (*getRoutes)(const XES&);  // function to get routes from type 'R'

 private:
  P* p;  // has to be the last (?)

 public:
  explicit NSSeqVRPExchange(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : getRoutes(_getRoutes), p(_p) {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = getRoutes(se);  // se.first.getR();
    int r = rand() % rep.size();
    if (rep.at(r).size() < 2)
      return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

    int c1 = rand() % rep.at(r).size();

    int c2;

    do {
      c2 = rand() % rep.at(r).size();
    } while (c1 == c2);

    // create exchange(p1,p2) move
    return uptr<Move<XES>>(new MOVE(getRoutes, r, c1, c2, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRPExchange with move: " << MOVE::idComponent();
    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRPEXCHANGE_HPP_
