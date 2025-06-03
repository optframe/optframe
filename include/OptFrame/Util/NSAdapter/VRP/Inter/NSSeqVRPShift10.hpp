// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSHIFT10_HPP_
#define OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSHIFT10_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

// Check if C++20 Concepts is supported
// // #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
// #ifdef OPTFRAME_USE_STD_CONCEPTS

namespace optframe {

/*
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<vector<T>>, ADS> S =
              CopySolution<vector<vector<T>>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPShift10 : public Move<XES> {
  using Routes = vector<vector<T>>;
  */
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPShift10Adapter : public Move<XES> {
  using XEv = typename XES::second_type;

 public:
  using Routes = std::vector<std::vector<int>>;
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 protected:
  int r1, r2;  // routes id's
  int cli;     // cli shifted in r1
  int pos;     // insertion position in r2

  P* problem;

 public:
  MoveVRPShift10Adapter(std::function<Routes&(const XES&)> _getRoutes, int _r1,
                        int _r2, int _cli, int _pos, P* _p = nullptr)
      : getRoutes{_getRoutes},
        r1(_r1),
        r2(_r2),
        cli(_cli),
        pos(_pos),
        problem(_p) {}

  virtual ~MoveVRPShift10Adapter() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_cli() { return cli; }

  int get_pos() { return pos; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = getRoutes(se);
    bool numRoutes = rep.size() >= 2;
    return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
  }

  // virtual void updateNeighStatus(ADS& ads) {}

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = getRoutes(se);
    // pegando o cliente
    int c = rep.at(r1).at(cli);

    // removendo os clientes de cada rota
    rep.at(r1).erase(rep.at(r1).begin() + cli);

    // fazendo a inserção
    rep.at(r2).insert(rep.at(r2).begin() + pos, c);
    return uptr<Move<XES>>(
        new MoveVRPShift10Adapter(getRoutes, r2, r1, pos, cli));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPShift10Adapter& m = (const MoveVRPShift10Adapter&)_m;
    return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
  }

  void print() const override {
    std::cout << "MoveVRPShift10Adapter( ";
    std::cout << r1 << " , ";
    std::cout << r2 << " , ";
    std::cout << cli << " , ";
    std::cout << pos << " )";
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Move<XES>::idComponent() << ":MoveVRPShift10Adapter";
    return ss.str();
  }
};

/*
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class NSIteratorVRPShift10 : public NSIterator<XES> {
  typedef vector<vector<T>> Routes;
*/
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true, class MOVE = MoveVRPShift10Adapter<XES, P>>
class NSIteratorVRPShift10Adapter : public NSIterator<XES> {
  using Routes = std::vector<std::vector<int>>;

 protected:
  uptr<Move<XES>> m;
  vector<uptr<Move<XES>>> moves;
  int index;  // index of moves
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;
  const Routes& r;

  P* p;  // has to be the last

 public:
  using MoveType = MOVE;
  NSIteratorVRPShift10Adapter(std::function<Routes&(const XES&)> _getRoutes,
                              const XES& se, P* _p = nullptr)
      : getRoutes{_getRoutes}, r{_getRoutes(se)}, p(_p) {
    m = nullptr;
    index = 0;
  }

  void first() override {
    for (int r1 = 0; r1 < (int)r.size(); r1++) {
      for (int r2 = 0; r2 < (int)r.size(); r2++) {
        if (r1 != r2) {
          for (int cli = 0; cli < (int)r.at(r1).size(); cli++) {
            for (int pos = 0; pos <= (int)r.at(r2).size(); pos++) {
              if constexpr (ADAPTER)
                moves.push_back(
                    uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, cli, pos, p)));
              else
                moves.push_back(uptr<Move<XES>>(new MOVE(r1, r2, cli, pos, p)));
            }
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
      std::cout << "NSSeqVRPShift10. Aborting." << std::endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    return m2;
  }
};

/*
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorVRPShift10<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPShift10 : public NSSeq<XES, XSH> {
  using Routes = std::vector<std::vector<T>>;
  */
template <XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM,
          bool ADAPTER = true,
          class NSITERATOR = NSIteratorVRPShift10Adapter<XES, P, ADAPTER>>
class NSSeqVRPShift10 : public NSSeq<XES> {
  using Routes = vector<vector<int>>;
  using MOVE = typename NSITERATOR::MoveType;

 public:
  // function to get routes from type 'R'
  std::function<Routes&(const XES&)> getRoutes;

 private:
  P* p;

 public:
  // template <typename T = typename XES::first_type,
  //           std::enable_if_t<!ADAPTER, int> = 0>
  // template <class = typename std::enable_if<not ADAPTER>::type>
  template <
      typename T = typename XES::first_type,
      std::enable_if_t<!ADAPTER && !std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPShift10(P* _p = nullptr) : p{_p} {}

  // (0) automatic when no conversion is needed
  template <typename T = typename XES::first_type,
            std::enable_if_t<ADAPTER && std::is_same_v<T, Routes>, bool> = true>
  explicit NSSeqVRPShift10(P* _p = nullptr)
      : getRoutes{[](const XES& se) -> Routes& {
          // hiding the innevitable const_cast from the user...
          // NOLINTNEXTLINE
          return const_cast<Routes&>(se.first);
        }},
        p{_p} {}

  // (2) legacy behavior: more efficient
  explicit NSSeqVRPShift10(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
      : getRoutes{_getRoutes}, p{_p} {}

  // explicit NSSeqVRPShift10(P* _p = nullptr) : p(_p) {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    // const Routes& rep = se.first.getR();
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

    int cli = rand() % rep.at(r1).size();

    int pos = rand() % (rep.at(r2).size() + 1);
    if constexpr (ADAPTER)
      return uptr<Move<XES>>(new MOVE(getRoutes, r1, r2, cli, pos, p));
    else
      return uptr<Move<XES>>(new MOVE(r1, r2, cli, pos, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    // auto& s = se.first;
    // return uptr<NSIterator<XES>>(new NSITERATOR(s.getR(), s.getADS(), p));
    if constexpr (ADAPTER)
      return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
    else
      return uptr<NSIterator<XES>>(new NSITERATOR(se, p));
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "NSSeqVRPShift10(ADAPTER=" << ADAPTER
       << ") with move: " << MOVE::idComponent();
    return ss.str();
  }

  // void print() const override { std::cout << toString() << std::endl; }
};

}  // namespace optframe

// #else
// #error C++20 is required for NSSeqVRPShift10.hpp
// #endif  // C++20

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSHIFT10_HPP_
