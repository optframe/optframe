// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRPOROPT1_HPP_
#define OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRPOROPT1_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSSeq.hpp>

namespace optframe {

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPOrOpt1 : public Move<XES> {
  using XEv = typename XES::second_type;
  typedef vector<vector<int>> Routes;

 public:
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 protected:
  int r;    // route id
  int c;    // client
  int pos;  // position

 private:
  P* problem;

 public:
  MoveVRPOrOpt1(std::function<Routes&(const XES&)> _getRoutes, int _r, int _c,
                int _pos, P* _problem = nullptr)
      : getRoutes(_getRoutes), r(_r), c(_c), pos(_pos), problem(_problem) {}

  int getRoute() { return r; }

  int getClient() { return c; }

  int getPosition() { return pos; }

  bool canBeApplied(const XES& se) override {
    bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
    return all_positive && (c != pos) && (c + 1 != pos);
  }

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);  // se.first.getR();
    int aux;
    int i;
    if (c < pos) {
      aux = rep.at(r).at(c);
      for (i = c; i < (pos - 1); i++) {
        rep.at(r).at(i) = rep.at(r).at(i + 1);
      }
      rep.at(r).at(i) = aux;
      return uptr<Move<XES>>(
          new MoveVRPOrOpt1<XES, P>(getRoutes, r, pos - 1, c));
    } else {
      aux = rep.at(r).at(c);
      for (i = c; i > pos; i--) {
        rep.at(r).at(i) = rep.at(r).at(i - 1);
      }
      rep.at(r).at(i) = aux;
      return uptr<Move<XES>>(
          new MoveVRPOrOpt1<XES, P>(getRoutes, r, pos, c + 1));
    }

    return uptr<Move<XES>>(new MoveVRPOrOpt1<XES, P>(getRoutes, -1, -1, -1));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPOrOpt1<XES, P>& m1 = (const MoveVRPOrOpt1<XES, P>&)_m;
    return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
  }

  void print() const override {
    std::cout << "MoveVRPOrOpt1";
    std::cout << "( " << r << " , " << c << " , " << pos << " )" << std::endl;
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES>::idComponent() << ":MoveVRPOrOpt1";
    return ss.str();
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRPOrOpt1<XES, P>>
class NSIteratorVRPOrOpt1 : public NSIterator<XES> {
  typedef vector<vector<int>> Routes;

 protected:
  uptr<Move<XES>> m;
  vector<uptr<Move<XES>>> moves;
  // index of moves
  int index;
  // function to get routes from type 'XES'
  std::function<Routes&(const XES&)> getRoutes;
  const Routes& rep;
  // has to be the last
  P* p;

 public:
  NSIteratorVRPOrOpt1(std::function<Routes&(const XES&)> getRoutes,
                      const XES& se, P* _p = nullptr)
      : m{nullptr}, index{0}, getRoutes(getRoutes), rep{getRoutes(se)}, p(_p) {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++) {
      for (int c = 0; c < (int)rep.at(r).size(); c++) {
        for (int pos = 0; pos <= (int)rep.at(r).size(); pos++) {
          if ((c != pos) && (c + 1 != pos)) {
            moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p)));
          }
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
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "VRPOrOpt1. Aborting." << std::endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRPOrOpt1<XES, P>,
          class NSITERATOR = NSIteratorVRPOrOpt1<XES, P, MOVE>>
class NSSeqVRPOrOpt1 : public NSSeq<XES> {
  typedef vector<vector<int>> Routes;

 public:
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 private:
  P* p;

 public:
  // (0) automatic when no conversion is needed
  template <typename T = typename XES::first_type,
            std::enable_if_t<std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPOrOpt1(P* _p = nullptr)
      : getRoutes{[](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(se.first);
        }},
        p{_p} {}

  explicit NSSeqVRPOrOpt1(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : getRoutes(_getRoutes), p(_p) {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = getRoutes(se);  // se.first.getR();
    int r = rand() % rep.size();

    if (rep.at(r).size() < 2)
      return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

    int c = rand() % rep.at(r).size();

    int pos = rand() % (rep.at(r).size() + 1);

    return uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "NSSeqVRPOrOpt1 with move: " << MOVE::idComponent();
    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRPOROPT1_HPP_
