// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSWAP1_1_HPP_
#define OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSWAP1_1_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

// Check if C++20 Concepts is supported
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

namespace optframe {

/*
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<vector<T>>, ADS> S =
              CopySolution<vector<vector<T>>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPSwap1_1 : public Move<XES> {
  typedef vector<vector<T>> Routes;
  */
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPSwap1_1Adapter : public Move<XES> {
 public:
  using Routes = vector<vector<int>>;
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 protected:
  int r1, r2;  // route 1 and route 2, respectively
  int c1, c2;  // client in route 1 and client in route 2, respectively

  P* problem;

 public:
  MoveVRPSwap1_1Adapter(std::function<Routes&(const XES&)> _getRoutes, int _r1,
                        int _r2, int _c1, int _c2, P* _problem = nullptr)
      : getRoutes{_getRoutes},
        r1(_r1),
        r2(_r2),
        c1(_c1),
        c2(_c2),
        problem(_problem) {}

  // virtual ~MoveVRPSwap1_1() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_c1() { return c1; }

  int get_c2() { return c2; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);
    bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 0) && (c2 >= 0);
    return all_positive && (rep.size() >= 2);
  }

  // virtual void updateNeighStatus(ADS& ads) {}

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);
    int aux;
    aux = rep.at(r1).at(c1);
    rep.at(r1).at(c1) = rep.at(r2).at(c2);
    rep.at(r2).at(c2) = aux;

    return uptr<Move<XES>>(
        new MoveVRPSwap1_1Adapter(getRoutes, r2, r1, c2, c1));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPSwap1_1Adapter& m1 = (const MoveVRPSwap1_1Adapter&)_m;
    return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2));
  }

  void print() const override {
    cout << "MoveVRPSwap1_1( ";
    cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2
         << "] client " << c2 << ")";
    cout << endl;
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES>::idComponent() << ":MoveVRPSwap1_1Adapter";
    return ss.str();
  }
};

/*
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPSwap1_1<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
          */
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true, class MOVE = MoveVRPSwap1_1Adapter<XES, P>>
class NSIteratorVRPSwap1_1Adapter : public NSIterator<XES> {
  typedef vector<vector<int>> Routes;

 protected:
  uptr<Move<XES>> m;
  vector<uptr<Move<XES>>> moves;
  int index;  // index of moves
  std::function<Routes&(const XES&)> getRoutes;
  const Routes& r;

  P* p;  // has to be the last

 public:
  using MoveType = MOVE;
  NSIteratorVRPSwap1_1Adapter(std::function<Routes&(const XES&)> _getRoutes,
                              const XES& se, P* _p = nullptr)
      : getRoutes{_getRoutes}, r{_getRoutes(se)}, p(_p) {
    m = nullptr;
    index = 0;
  }

  void first() override {
    for (int r1 = 0; r1 < r.size() - 2; r1++)
      for (int c1 = 0; c1 < r.at(r1).size(); c1++)
        for (int r2 = r1 + 1; r2 < r.size() - 1; r2++)
          for (int c2 = 0; c2 < r.at(r2).size(); c2++) {
            if constexpr (ADAPTER)
              moves.push_back(
                  uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, c1, c2, p)));
            else
              moves.push_back(uptr<Move<XES>>(new MOVE(r1, r2, c1, c2, p)));
          }

    if (moves.size() > 0)
      m = std::move(moves[index]);
    else
      m = nullptr;
  }

  void next() override {
    index++;
    if (index < moves.size())
      m = std::move(moves[index]);
    else
      m = nullptr;
  }

  bool isDone() override { return m == nullptr; }

  uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPSwap1_1. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);

    return m2;
  }
};

/*
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPSwap1_1<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorVRPSwap1_1<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
          */
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true,
          class NSITERATOR = NSIteratorVRPSwap1_1Adapter<XES, P, ADAPTER>>
class NSSeqVRPSwap1_1 : public NSSeq<XES> {
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
  explicit NSSeqVRPSwap1_1(P* _p = nullptr) : p{_p} {}

  // (0) automatic when no conversion is needed
  template <typename T = typename XES::first_type,
            std::enable_if_t<ADAPTER && std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPSwap1_1(P* _p = nullptr)
      : getRoutes{[](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(se.first);
        }},
        p{_p} {}

  // (2) legacy behavior: more efficient
  explicit NSSeqVRPSwap1_1(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : getRoutes{_getRoutes}, p{_p} {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = getRoutes(se);

    if (rep.size() < 2) {
      if constexpr (ADAPTER)
        return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, -1, p));
      else
        return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int r1 = rand() % rep.size();

    if (rep.at(r1).size() == 0) {
      if constexpr (ADAPTER)
        return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, -1, p));
      else
        return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int r2;

    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    if (rep.at(r2).size() == 0) {
      if constexpr (ADAPTER)
        return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, -1, p));
      else
        return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int c1 = rand() % rep.at(r1).size();
    int c2 = rand() % rep.at(r2).size();

    if constexpr (ADAPTER)
      return uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, c1, c2, p));
    else
      return uptr<Move<XES>>(new MOVE(r1, r2, c1, c2, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    auto& s = se.first;
    if constexpr (ADAPTER)
      return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
    else
      return uptr<NSIterator<XES>>(new NSITERATOR(se, p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRPSwap1_1(ADAPTER=" << ADAPTER
       << ") with move: " << MOVE::idComponent();
    return ss.str();
  }

  virtual void print() { cout << toString() << endl; }
};

}  // namespace optframe

// #else
// #error C++20 is required
// #endif  // C++20

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSWAP1_1_HPP_
