// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/NSSeqShift.h>

using namespace PN;

// ============ MoveShift ==============

bool MoveShift::canBeApplied(const ESolutionPN& se) {
  const RepPN& rep = se.first.getR();
  if (i >= (int)rep.size() - 1 || i < 0) return false;
  return true;
}

uptr<Move<ESolutionPN>> MoveShift::apply(ESolutionPN& se) {
  RepPN& rep = se.first.getR();
  rep[i] = !(rep[i]);

  return uptr<Move<ESolutionPN>>{new MoveShift(i, pPN)};
}

op<EvaluationPN> MoveShift::cost(const ESolutionPN& se, bool allowEstimate) {
  return NULL;
  // Implement if a more efficient evaluation of the move is available
  // double move_cost = ...;
  // double move_cost_infeasible = ...;
  // return new MoveCost<>(move_cost, move_cost_infeasible);
}

// ============ NSIteratorShift ==============

void NSIteratorShift::first() { i = 0; };

void NSIteratorShift::next() { i++; };

bool NSIteratorShift::isDone() { return (i > (int)rep.size() - 1); };

uptr<Move<ESolutionPN>> NSIteratorShift::current() {
  return uptr<Move<ESolutionPN>>{new MoveShift(i, pPN)};
};

// ============ NSSeqShift ==============

uptr<Move<ESolutionPN>> NSSeqShift::randomMove(const ESolutionPN& se) {
  const RepPN& rep = se.first.getR();
  int i1 = rg.rand(rep.size());

  return uptr<Move<ESolutionPN>>{new MoveShift(i1, pPN)};
}
