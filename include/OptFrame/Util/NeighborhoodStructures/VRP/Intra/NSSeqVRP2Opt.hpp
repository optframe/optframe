// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRP2OPT_HPP_
#define OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRP2OPT_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/NSSeq.hpp>

namespace optframe {

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRP2Opt : public Move<XES> {
  using XEv = typename XES::second_type;

 public:
  using Routes = std::vector<std::vector<int>>;
  Routes& (*getRoutes)(const XES&);  // function to get routes from type 'R'

 protected:
  int r;       // route id
  int p1, p2;  // position 1 and position 2, respectively
  P* problem;

 public:
  MoveVRP2Opt(Routes& (*_getRoutes)(const XES&), int _r, int _p1, int _p2,
              P* _problem = nullptr)
      : getRoutes(_getRoutes), r(_r), p1(_p1), p2(_p2), problem(_problem) {}

  int get_p1() { return p1; }

  int get_p2() { return p2; }

  int get_r() { return r; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);
    bool all_positive = (p1 >= 0) && (p2 >= 0) && (r >= 0);
    return all_positive && (rep.at(r).size() >= 2);
  }

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);  // se.first.getR();
    int small = 0, bigger = 0;
    if (p1 <= p2) {
      small = p1;
      bigger = p2;
    } else {
      small = p2;
      bigger = p1;
    }

    reverse(rep.at(r).begin() + small, rep.at(r).begin() + bigger);

    return uptr<Move<XES>>(new MoveVRP2Opt(getRoutes, r, p1, p2));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRP2Opt& m1 = (const MoveVRP2Opt&)_m;
    return ((m1.p1 == p1) && (m1.p2 == p2) && (m1.r == r));
  }

  void print() const override {
    cout << "MoveVRP2Opt( route: " << r << " : ";
    cout << " edge " << p1 << " <=>  edge " << p2 << " )";
    cout << endl;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>>
class NSIteratorVRP2Opt : public NSIterator<XES> {
  using Routes = std::vector<std::vector<int>>;

 protected:
  // storing general move
  uptr<Move<XES>> m;
  // current move index
  int index;
  // strategy: storing general moves
  std::vector<uptr<Move<XES>>> moves;
  // function to get routes from type 'R'
  Routes& (*getRoutes)(const XES&);
  //
  const Routes& rep;

  P* p;  // has to be the last

 public:
  NSIteratorVRP2Opt(Routes& (*getRoutes)(const XES&), const XES& se,
                    P* _p = nullptr)
      : m{nullptr}, index{0}, getRoutes(getRoutes), rep{getRoutes(se)}, p(_p) {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++) {
      int tam = rep.at(r).size() - 2;
      for (int p1 = 0; p1 < tam; p1++) {
        for (int p2 = p1 + 2; p2 < (int)rep.at(r).size(); p2++) {
          moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, p1, p2, p)));
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
      cout << "NSSeqVRP2Opt. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>,
          class NSITERATOR = NSIteratorVRP2Opt<XES, P, MOVE>>
class NSSeqVRP2Opt : public NSSeq<XES> {
  typedef vector<vector<int>> Routes;

 public:
  Routes& (*getRoutes)(const XES&);  // function to get routes from type 'R'

 private:
  P* p;

 public:
  explicit NSSeqVRP2Opt(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : getRoutes(_getRoutes), p(_p) {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = getRoutes(se);  // se.first.getR();
    int r = rand() % rep.size();
    if (rep.at(r).size() < 3)
      return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

    int p1 = rand() % (rep.at(r).size() + 1);

    int p2 = 0;

    do {
      p2 = rand() % (rep.at(r).size() + 1);
    } while (abs(p1 - p2) < 2);

    // create 2-opt(p1,p2) move
    return uptr<Move<XES>>(new MOVE(getRoutes, r, p1, p2, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRP2Opt with move: " << MOVE::idComponent();
    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTRA_NSSEQVRP2OPT_HPP_
