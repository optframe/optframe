// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/NSSeqSwap.h>

using namespace PN;

// ============ MoveSwap ==============

bool MoveSwap::canBeApplied(const ESolutionPN& se) {
  const RepPN& rep = se.first.getR();
  if (i >= (int)rep.size() - 1 || i < 0) return false;
  if (j >= (int)rep.size() - 1 || j < 0) return false;
  if (rep[i] == rep[j]) return false;
  return true;
}

uptr<Move<ESolutionPN>> MoveSwap::apply(ESolutionPN& se) {
  RepPN& rep = se.first.getR();
  int aux;

  aux = rep[i];
  rep[i] = rep[j];
  rep[j] = aux;

  return uptr<Move<ESolutionPN>>{new MoveSwap(j, i, pPN)};
}

op<EvaluationPN> MoveSwap::cost(const ESolutionPN& se, bool allowEstimate) {
  return NULL;
  // Implement if a more efficient evaluation of the move is available
  // double move_cost = ...;
  // double move_cost_infeasible = ...;
  // return new MoveCost<>(move_cost, move_cost_infeasible);
}

// ============ NSIteratorSwap ==============

void NSIteratorSwap::first() {
  i = 0;
  j = 1;
};

void NSIteratorSwap::next() {
  j++;
  if (j > (int)rep.size() - 1) {
    i++;
    j = 1;
  }
};

bool NSIteratorSwap::isDone() { return (i >= (int)rep.size() - 1); };

uptr<Move<ESolutionPN>> NSIteratorSwap::current() {
  return uptr<Move<ESolutionPN>>{new MoveSwap(i, j, pPN)};
};

// ============ NSSeqSwap ==============

uptr<Move<ESolutionPN>> NSSeqSwap::randomMove(const ESolutionPN& se) {
  const RepPN& rep = se.first.getR();
  int i1 = rg.rand(rep.size() - 1);
  int j1 = rg.rand(rep.size() - 1);

  return uptr<Move<ESolutionPN>>{new MoveSwap(i1, j1, pPN)};
}
