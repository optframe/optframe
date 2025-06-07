// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTER_NSSEQVRPSHIFT20_HPP_
#define OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTER_NSSEQVRPSHIFT20_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSSeq.hpp>

namespace optframe {
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPShift20 : public Move<vector<vector<T>>, ADS, DS> {
  typedef vector<vector<T>> Routes;

 private:
  int r1, r2;  // routes id's
  int cli;     // cli shifted in r1
  int pos;     // insertion position in r2

  OPTFRAME_DEFAULT_PROBLEM* problem;

 public:
  MoveVRPShift20(int _r1, int _r2, int _cli, int _pos,
                 OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : r1(_r1), r2(_r2), cli(_cli), pos(_pos), problem(_problem) {}

  virtual ~MoveVRPShift20() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_cli() { return cli; }

  int get_pos() { return pos; }

  bool canBeApplied(const Routes& rep, const ADS*) override {
    bool numRoutes = rep.size() >= 2;
    return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
  }

  Move<Routes, ADS, DS>* apply(Routes& rep, ADS*) override {
    // pegando o cliente
    int c = rep.at(r1).at(cli);
    int c2 = rep.at(r1).at(cli + 1);

    // removendo os clientes de cada rota
    rep.at(r1).erase(rep.at(r1).begin() + cli);
    rep.at(r1).erase(rep.at(r1).begin() + cli);

    // fazendo a inserção
    rep.at(r2).insert(rep.at(r2).begin() + pos, c2);
    rep.at(r2).insert(rep.at(r2).begin() + pos, c);

    return new MoveVRPShift20<T, ADS, DS>(r2, r1, pos, cli);
  }

  virtual bool operator==(const Move<Routes>& _m) const {
    const MoveVRPShift20<T, ADS, DS>& m = (const MoveVRPShift20<T, ADS, DS>&)_m;
    return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
  }

  void print() const override {
    std::cout << "MoveVRPShift20( ";
    std::cout << r1 << " , ";
    std::cout << r2 << " , ";
    std::cout << cli << " , ";
    std::cout << pos << " )";
  }
};

template <
    class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
    class MOVE = MoveVRPShift20<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPShift20 : public NSIterator<vector<vector<T>>, ADS, DS> {
  typedef vector<vector<T>> Routes;

 private:
  MoveVRPShift20<T, ADS, DS>* m;
  vector<uptr<MoveVRPShift20<T, ADS, DS>>> moves;
  int index;  // index of moves
  const Routes& r;
  // has to be the last
  P* p;

 public:
  explicit NSIteratorVRPShift20(const Routes& _r, P* _p = nullptr)
      : m{nullptr}, index{0}, r{_r}, p{_p} {}

  void first() override {
    for (int r1 = 0; r1 < r.size(); r1++) {
      for (int r2 = 0; r2 < r.size(); r2++) {
        if (r1 != r2) {
          for (int cli = 0; cli < ((int)r.at(r1).size()) - 1; cli++) {
            for (int pos = 0; pos <= r.at(r2).size(); pos++) {
              moves.push_back(
                  uptr<Move<ESolutionHFMVRP>>(new MOVE(r1, r2, cli, pos, p)));
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

  Move<Routes, ADS, DS>* current() override {
    if (isDone()) {
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "NSSeqVRPShift20. Aborting." << std::endl;
      exit(1);
    }

    return m;
  }
};

template <
    class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
    class MOVE = MoveVRPShift20<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM,
    class NSITERATOR = NSIteratorVRPShift20<T, ADS, DS, MOVE, P>>
class NSSeqVRPShift20 : public NSSeq<vector<vector<T>>, ADS, DS> {
  typedef vector<vector<T>> Routes;

 private:
  P* p;  // has to be the last

 public:
  explicit NSSeqVRPShift20(P* _p = nullptr) : p(_p) {}

  uptr<Move<Routes, ADS, DS>> randomMove(const Routes& rep,
                                         const ADS*) override {
    if (rep.size() < 2) return *new MOVE(-1, -1, -1, -1, p);

    int r1 = rand() % rep.size();
    if (rep.at(r1).size() < 2) {
      return uptr<Move<Routes, ADS, DS>>(MOVE(-1, -1, -1, -1, p));
    }

    int r2;
    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    int cli = rand() % (rep.at(r1).size() - 1);

    int pos = rand() % (rep.at(r2).size() + 1);
    return uptr<Move<Routes, ADS, DS>>(
        MOVE(r1, r2, cli, pos, p));  // return a random move
  }

  uptr<NSIteratorVRPShift20<T, ADS, DS, MOVE, P>> getIterator(
      const Routes& r, const ADS*) override {
    return new NSITERATOR(r, p);
  }

  virtual void print() {
    std::cout << "NSSeqVRPShift20 with move: " << MOVE::idMoveComponent();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_UTIL_NEIGHBORHOODSTRUCTURES_VRP_INTER_NSSEQVRPSHIFT20_HPP_
