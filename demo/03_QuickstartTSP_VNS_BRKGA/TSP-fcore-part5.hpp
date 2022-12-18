
std::pair<int, int> fApplySwap(const std::pair<int, int>& moveData,
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
using MoveSwap = FMove<std::pair<int, int>, ESolutionTSP>;

uptr<Move<ESolutionTSP>> makeMoveSwap(int i, int j) {
  return uptr<Move<ESolutionTSP>>(new MoveSwap{make_pair(i, j), fApplySwap});
}