// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_HPP_
#define OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/NSSeq.hpp>

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
#else
template <typename XES, class P = OPTFRAME_DEFAULT_PROBLEM>
#endif
class MoveVRP2Opt : public Move<XES> {
  using XEv = typename XES::second_type;

 public:
  using Routes = std::vector<std::vector<int>>;
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 protected:
  int r;       // route id
  int p1, p2;  // position 1 and position 2, respectively
  P* problem;

 public:
  MoveVRP2Opt(std::function<Routes&(const XES&)> getRoutes, int r, int p1,
              int p2, P* problem = nullptr)
      : getRoutes{getRoutes}, r{r}, p1{p1}, p2{p2}, problem{problem} {}

  int get_p1() { return p1; }

  int get_p2() { return p2; }

  int get_r() { return r; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);
    bool all_positive = (p1 >= 0) && (p2 >= 0) && (r >= 0);
    return all_positive && (rep.at(r).size() >= 2);
  }

  uptr<Move<XES>> apply(XES& se) override {
    // buggy msvc line
    Routes& rep = getRoutes(se);
    //
    //
    int small = 0;
    int bigger = 0;
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

  std::string toString() const override {
    std::stringstream ss;
    ss << "MoveVRP2Opt( route: " << r << " : ";
    ss << " edge " << p1 << " <=>  edge " << p2 << " )";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES>::idComponent() << ":MoveVRP2Opt";
    return ss.str();
  }

  // void print() const override { cout << toString() << endl; }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>>
#else
template <typename XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>>
#endif
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
  std::function<Routes&(const XES&)> getRoutes;
  //
  const Routes& rep;

  P* p;  // has to be the last

 public:
  NSIteratorVRP2Opt(std::function<Routes&(const XES&)> getRoutes, const XES& se,
                    P* p = nullptr)
      : m{nullptr}, index{0}, getRoutes{getRoutes}, rep{getRoutes(se)}, p{p} {}

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

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>,
          class NSITERATOR = NSIteratorVRP2Opt<XES, P, MOVE>>
#else
template <typename XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          class MOVE = MoveVRP2Opt<XES, P>,
          class NSITERATOR = NSIteratorVRP2Opt<XES, P, MOVE>>
#endif
class NSSeqVRP2Opt : public NSSeq<XES> {
  typedef std::vector<std::vector<int>> Routes;

 public:
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 private:
  P* p;

  using fConstRoute = std::function<const Routes&(const XES&)>;

 public:
// (0) automatic when no conversion is needed
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  explicit NSSeqVRP2Opt(
      P* p = nullptr) requires std::is_same_v<typename XES::first_type, Routes>
      : getRoutes{[](const XES& se) -> Routes& {
        // hiding the innevitable const_cast from the user...
        // NOLINTNEXTLINE
        return const_cast<Routes&>(se.first);
      }},
        p{p} {}
#else
#ifndef _MSC_VER  // NO MSVC
  // (0) automatic when no conversion is needed
  template <typename T = typename XES::first_type,
            std::enable_if_t<std::is_same_v<T, Routes>>* = nullptr>
  explicit NSSeqVRP2Opt(P* p = nullptr)
      : getRoutes{[](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(se.first);
        }},
        p{p} {}
#endif            // no MSVC
#endif            // no C++20

  // (1) updated behavior: easier, but less efficient
  // reversed parameters to allow differentiation
  explicit NSSeqVRP2Opt(P* p, fConstRoute func)
      : getRoutes{[func](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(func(se));
        }},
        p{p} {}

  // (2) legacy behavior: more efficient
  explicit NSSeqVRP2Opt(std::function<Routes&(const XES&)> getRoutes,
                        P* p = nullptr)
      : getRoutes{getRoutes}, p{p} {}

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

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTRA_NSSEQVRP2OPT_HPP_
