// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_NSSEQSwap_H_
#define PN_NSSEQSwap_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace PN {

class MoveSwap : public Move<ESolutionPN> {
 private:
  // MOVE PARAMETERS
  int i, j;
  ProblemInstance& pPN;

 public:
  MoveSwap(int _i, int _j, ProblemInstance& _pPN) : i(_i), j(_j), pPN(_pPN) {}

  virtual ~MoveSwap() {}

  void print() const override {
    cout << id() << " with params: '" << i << "," << j << endl;
  }

  std::string id() const override {
    return Move<ESolutionPN>::idComponent().append(":MoveSwap");
  }

  bool operator==(const Move<ESolutionPN>& _m) const {
    const MoveSwap& m = (const MoveSwap&)_m;
    return (i == m.i) && (j == m.j);
  }

  // Implement these methods in the .cpp file

  bool canBeApplied(const ESolutionPN& se) override;

  uptr<Move<ESolutionPN>> apply(ESolutionPN& se) override;

  op<EvaluationPN> cost(const ESolutionPN& se, bool allowEstimate) override;
};

class NSIteratorSwap : public NSIterator<ESolutionPN> {
 private:
  // ITERATOR PARAMETERS
  int i, j;
  const RepPN& rep;
  ProblemInstance& pPN;

 public:
  NSIteratorSwap(ProblemInstance& _pPN, const ESolutionPN& se)
      : rep{se.first.getR()}, pPN(_pPN) {}

  virtual ~NSIteratorSwap() {}

  // Implement these methods in the .cpp file

  void first() override;
  void next() override;
  bool isDone() override;
  uptr<Move<ESolutionPN>> current() override;
};

class NSSeqSwap : public NSSeq<ESolutionPN> {
 private:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  ProblemInstance& pPN;  // problem instance data
  RandGen& rg;           // random number generator

 public:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  NSSeqSwap(ProblemInstance& _pPN, RandGen& _rg) : pPN(_pPN), rg(_rg) {}

  virtual ~NSSeqSwap() {}

  void print() const override { cout << "NSSeqSwap" << endl; }

  std::string id() const override {
    return NSSeq<ESolutionPN>::idComponent().append(":NSSeqSwap");
  }

  uptr<NSIterator<ESolutionPN>> getIterator(const ESolutionPN& se) override {
    // return an iterator to the neighbors of 'rep'
    return uptr<NSIterator<ESolutionPN>>{
        new NSIteratorSwap(pPN, se)};  // ADD POSSIBLE ITERATOR PARAMETERS
  }

  // Implement this method in the .cpp file

  uptr<Move<ESolutionPN>> randomMove(const ESolutionPN& se);

  uptr<Move<ESolutionPN>> validMove(const ESolutionPN& se) {
    for (unsigned i = 0; i < 100; i++) {
      uptr<Move<ESolutionPN>> m = randomMove(se);
      if (m->canBeApplied(se)) return m;
    }
    return NULL;
  }
};
}  // namespace PN

#endif /*PN_NSSEQSwap_H_*/
