// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_NSSEQShift_H_
#define PN_NSSEQShift_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace PN {

class MoveShift : public Move<ESolutionPN> {
 private:
  // MOVE PARAMETERS
  int i;
  ProblemInstance& pPN;

 public:
  MoveShift(int _i, ProblemInstance& _pPN) : i(_i), pPN(_pPN) {}

  virtual ~MoveShift() {}

  void print() const override {
    cout << id() << " with params: '" << i << "'" << endl;
  }

  std::string id() const override {
    return Move<ESolutionPN>::idComponent().append(":MoveShift");
  }

  bool operator==(const Move<ESolutionPN>& _m) const {
    const MoveShift& m = (const MoveShift&)_m;
    return (i == m.i);
  }

  // Implement these methods in the .cpp file

  // bool canBeApplied(const RepPN& rep, const MY_ADS*) override;
  bool canBeApplied(const ESolutionPN& se) override;

  uptr<Move<ESolutionPN>> apply(ESolutionPN& se) override;

  op<EvaluationPN> cost(const ESolutionPN& se, bool allowEstimate) override;
};

class NSIteratorShift : public NSIterator<ESolutionPN> {
 private:
  // ITERATOR PARAMETERS
  int i;
  const ESolutionPN& se;
  const RepPN& rep;
  ProblemInstance& pPN;

 public:
  NSIteratorShift(ProblemInstance& _pPN, const ESolutionPN& _se)
      : se{_se}, rep(_se.first.getR()), pPN(_pPN) {}

  virtual ~NSIteratorShift() {}

  // Implement these methods in the .cpp file

  void first() override;
  void next() override;
  bool isDone() override;
  uptr<Move<ESolutionPN>> current() override;
};

class NSSeqShift : public NSSeq<ESolutionPN> {
 private:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  ProblemInstance& pPN;  // problem instance data
  RandGen& rg;           // random number generator

 public:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  NSSeqShift(ProblemInstance& _pPN, RandGen& _rg) : pPN(_pPN), rg(_rg) {}

  virtual ~NSSeqShift() {}

  void print() const override { cout << "NSSeqShift" << endl; }

  std::string id() const override {
    return NSSeq<ESolutionPN>::idComponent().append(":NSSeqShift");
  }

  uptr<NSIterator<ESolutionPN>> getIterator(const ESolutionPN& se) {
    // return an iterator to the neighbors of 'rep'
    return uptr<NSIterator<ESolutionPN>>{
        new NSIteratorShift(pPN, se)};  // ADD POSSIBLE ITERATOR PARAMETERS
  }

  // Implement this method in the .cpp file

  uptr<Move<ESolutionPN>> randomMove(const ESolutionPN& se) override;

  uptr<Move<ESolutionPN>> validMove(const ESolutionPN& se) {
    for (unsigned i = 0; i < 100; i++) {
      uptr<Move<ESolutionPN>> m = randomMove(se);
      if (m->canBeApplied(se)) return m;
    }
    return NULL;
  }
};
}  // namespace PN

#endif /*PN_NSSEQShift_H_*/
