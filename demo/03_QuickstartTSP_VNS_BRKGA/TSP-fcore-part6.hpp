
uptr<Move<ESolutionTSP>> fRandomSwap(sref<ProblemContext> pTSP,
                                     const ESolutionTSP& se) {
  int i = rand() % pTSP->n;
  int j = i;
  while (j <= i) {
    i = rand() % pTSP->n;
    j = rand() % pTSP->n;
  }
  return uptr<Move<ESolutionTSP>>(makeMoveSwap(pTSP, i, j));
}

// Swap move (NS)
// FNS<ESolutionTSP> nsswap{fRandomSwap};
