
std::pair<int, int> fApplySwap(sref<ProblemContext>,
                               const std::pair<int, int>& moveData,
                               ESolutionTSP& se) {
  int i = moveData.first;
  int j = moveData.second;
  // perform swap of clients i and j
  int aux = se.first[j];
  se.first[j] = se.first[i];
  se.first[i] = aux;
  return std::pair<int, int>(j, i);  // return a reverse move ('undo' move)s
}

// Swap move
using MoveSwap = FMoveP<std::pair<int, int>, ESolutionTSP, ProblemContext>;

uptr<Move<ESolutionTSP>> makeMoveSwap(sref<ProblemContext> p, int i, int j) {
  return uptr<Move<ESolutionTSP>>(new MoveSwap{p, make_pair(i, j), fApplySwap});
}