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
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

// Check if C++20 Concepts is supported
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

namespace optframe {

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<vector<T>>, ADS> S =
              CopySolution<vector<vector<T>>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPShift10 : public Move<XES> {
  using Routes = vector<vector<T>>;

 protected:
  int r1, r2;  // routes id's
  int cli;     // cli shifted in r1
  int pos;     // insertion position in r2

  OPTFRAME_DEFAULT_PROBLEM* problem;

 public:
  MoveVRPShift10(int _r1, int _r2, int _cli, int _pos,
                 OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : r1(_r1), r2(_r2), cli(_cli), pos(_pos), problem(_problem) {}

  virtual ~MoveVRPShift10() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_cli() { return cli; }

  int get_pos() { return pos; }

  bool canBeApplied(const XES& se) override {
    const Routes& rep = se.first.getR();
    bool numRoutes = rep.size() >= 2;
    return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
  }

  virtual void updateNeighStatus(ADS& ads) {}

  uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = se.first.getR();
    // pegando o cliente
    int c = rep.at(r1).at(cli);

    // removendo os clientes de cada rota
    rep.at(r1).erase(rep.at(r1).begin() + cli);

    // fazendo a inserção
    rep.at(r2).insert(rep.at(r2).begin() + pos, c);
    return uptr<Move<XES>>(new MoveVRPShift10(r2, r1, pos, cli));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPShift10& m = (const MoveVRPShift10&)_m;
    return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
  }

  void print() const override {
    cout << "MoveVRPShift10( ";
    cout << r1 << " , ";
    cout << r2 << " , ";
    cout << cli << " , ";
    cout << pos << " )";
  }
};

template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class NSIteratorVRPShift10 : public NSIterator<XES> {
  typedef vector<vector<T>> Routes;

 protected:
  uptr<Move<XES>> m;
  vector<uptr<Move<XES>>> moves;
  int index;  // index of moves
  const Routes& r;

  P* p;  // has to be the last

 public:
  NSIteratorVRPShift10(const Routes& _r, const ADS& _ads, P* _p = nullptr)
      : r(_r), p(_p) {
    m = nullptr;
    index = 0;
  }

  void first() override {
    for (int r1 = 0; r1 < r.size(); r1++) {
      for (int r2 = 0; r2 < r.size(); r2++) {
        if (r1 != r2) {
          for (int cli = 0; cli < r.at(r1).size(); cli++) {
            for (int pos = 0; pos <= r.at(r2).size(); pos++) {
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
    if (index < moves.size())
      m = std::move(moves[index]);
    else
      m = nullptr;
  }

  bool isDone() override { return m == nullptr; }

  uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPShift10. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    return m2;
  }
};

template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorVRPShift10<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPShift10 : public NSSeq<XES, XSH> {
  using Routes = std::vector<std::vector<T>>;

 private:
  P* p;

 public:
  explicit NSSeqVRPShift10(P* _p = nullptr) : p(_p) {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = se.first.getR();
    if (rep.size() < 2) return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));

    int r1 = rand() % rep.size();
    if (rep.at(r1).size() == 0) {
      return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int r2;
    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    int cli = rand() % rep.at(r1).size();

    int pos = rand() % (rep.at(r2).size() + 1);
    return uptr<Move<XES>>(
        new MOVE(r1, r2, cli, pos, p));  // return a random move
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    XSolution AUTO_CONCEPTS& s = se.first;
    return uptr<NSIterator<XES>>(new NSITERATOR(s.getR(), s.getADS(), p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRPShift10 with move: " << MOVE::idComponent();
    return ss.str();
  }

  void print() const override {
    cout << "NSSeqVRPShift10 with move: " << MOVE::idComponent();
  }
};

}  // namespace optframe

#else
#error C++20 is required for NSSeqVRPShift10.hpp
#endif  // C++20

#endif  // OPTFRAME_UTIL_NSADAPTER_VRP_INTER_NSSEQVRPSHIFT10_HPP_
