// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPCROSS_HPP_
#define OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPCROSS_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

// Framework includes
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSSeq.hpp>

// Check if C++20 Concepts is supported
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

namespace optframe {

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPCrossAdapter : public Move<XES> {
 public:
  using Routes = vector<vector<int>>;
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 protected:
  int r1, r2;  // route 1 and route 2;
  int p1, p2;  // position 1 and position 2 of cross in r1 end r2, respectively

  OPTFRAME_DEFAULT_PROBLEM* problem;

 public:
  MoveVRPCrossAdapter(std::function<Routes&(const XES&)> _getRoutes, int _r1,
                      int _r2, int _p1, int _p2,
                      OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : getRoutes{_getRoutes},
        r1(_r1),
        r2(_r2),
        p1(_p1),
        p2(_p2),
        problem(_problem) {}

  virtual ~MoveVRPCrossAdapter() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_p1() { return p1; }

  int get_p2() { return p2; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);
    bool all_positive = (r1 >= 0) && (r2 >= 0) && (p1 >= 0) && (p2 >= 0);
    return all_positive && (rep.size() >= 2) && (rep.at(r1).size() >= 0) &&
           (rep.at(r2).size() >= 0);
  }

  // virtual void updateNeighStatus(ADS& ads);

  // Move<Routes, ADS, DS>* apply(Routes& rep, ADS*) override {
  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);
    vector<int> cross_r1, cross_r2;

    // copiando as partes que serão trocadas
    cross_r1.assign(rep.at(r1).begin() + p1, rep.at(r1).end());
    cross_r2.assign(rep.at(r2).begin() + p2, rep.at(r2).end());

    // removendo as partes copiadas
    rep.at(r1).erase(rep.at(r1).begin() + p1, rep.at(r1).end());
    rep.at(r2).erase(rep.at(r2).begin() + p2, rep.at(r2).end());

    // inserindo as novas partes

    rep.at(r1).insert(rep.at(r1).end(), cross_r2.begin(), cross_r2.end());
    rep.at(r2).insert(rep.at(r2).end(), cross_r1.begin(), cross_r1.end());

    // p->p, r1->r2, r2->r1, c1->c2, c2->c1, reverse->reverse,
    return uptr<Move<XES>>(new MoveVRPCrossAdapter(getRoutes, r1, r2, p1, p2));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPCrossAdapter& m1 = (const MoveVRPCrossAdapter&)_m;
    return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.p1 == p1) && (m1.p2 == p2));
  }

  void print() const override {
    cout << "MoveVRPCross( ";
    cout << r1 << ", " << r2 << ", " << p1 << ", " << p2 << " )";
    cout << endl;
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES>::idComponent() << ":MoveVRPCrossAdapter";
    return ss.str();
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true, class MOVE = MoveVRPCrossAdapter<XES, P>>
class NSIteratorVRPCrossAdapter : public NSIterator<XES> {
  typedef vector<vector<int>> Routes;

 private:
  uptr<Move<XES>> m;              // general move
  vector<uptr<Move<XES>>> moves;  // general moves
  std::function<Routes&(const XES&)> getRoutes;
  int index;  // index of moves
  const Routes& r;

  P* p;  // has to be the last

 public:
  using MoveType = MOVE;
  explicit NSIteratorVRPCrossAdapter(
      std::function<Routes&(const XES&)> _getRoutes, const XES& se,
      P* _p = nullptr)
      : getRoutes{_getRoutes}, r{_getRoutes(se)}, p{_p} {
    m = nullptr;
    index = 0;
  }

  void first() override {
    for (int r1 = 0; r1 < (int)r.size() - 1; r1++) {
      for (int r2 = r1 + 1; r2 < (int)r.size(); r2++) {
        for (int p1 = 0; p1 <= (int)r.at(r1).size(); p1++) {
          for (int p2 = 0; p2 <= (int)r.at(r2).size(); p2++) {
            if constexpr (ADAPTER)
              moves.push_back(
                  uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, p1, p2, p)));
            else
              moves.push_back(uptr<Move<XES>>(new MOVE(r1, r2, p1, p2, p)));
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
    if (index < (int)moves.size()) {
      m = std::move(moves[index]);
    } else {
      m = nullptr;
    }
  }

  bool isDone() override { return m == nullptr; }

  uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPCross. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);

    return m2;
  }
};

template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true,
          class NSITERATOR = NSIteratorVRPCrossAdapter<XES, P, ADAPTER>>
class NSSeqVRPCross : public NSSeq<XES> {
  using Routes = vector<vector<int>>;
  using MOVE = typename NSITERATOR::MoveType;

 private:
  P* p;  // has to be the last

 public:
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

  template <
      typename T = typename XES::first_type,
      std::enable_if_t<!ADAPTER && !std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPCross(P* _p = nullptr) : p{_p} {}

  // (0) automatic when no conversion is needed
  template <typename T = typename XES::first_type,
            std::enable_if_t<ADAPTER && std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPCross(P* _p = nullptr)
      : p{_p}, getRoutes{[](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(se.first);
        }} {}

  // (2) legacy behavior: more efficient
  explicit NSSeqVRPCross(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : p{_p}, getRoutes{_getRoutes} {}

  virtual ~NSSeqVRPCross() {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = getRoutes(se);

    if (rep.size() < 2) {
      if constexpr (ADAPTER)
        return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, -1, p));
      else
        return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int r1 = rand() % rep.size();

    int r2;

    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    if ((rep.at(r2).size() == 0) && (rep.at(r1).size() == 0)) {
      if constexpr (ADAPTER)
        return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, -1, p));
      else
        return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int p1 = rand() % (rep.at(r1).size() + 1);
    int p2 = rand() % (rep.at(r2).size() + 1);

    if constexpr (ADAPTER)
      return uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, p1, p2, p));
    else
      return uptr<Move<XES>>(new MOVE(r1, r2, p1, p2, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    if constexpr (ADAPTER)
      return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
    else
      return uptr<NSIterator<XES>>(new NSITERATOR(se, p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRPCross(ADAPTER=" << ADAPTER
       << ") with move: " << MOVE::idComponent();
    return ss.str();
  }

  void print() const override { cout << toString() << endl; }
};

}  // namespace optframe

// #else
// #error C++20 is required
// #endif  // C++20

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPCROSS_HPP_
