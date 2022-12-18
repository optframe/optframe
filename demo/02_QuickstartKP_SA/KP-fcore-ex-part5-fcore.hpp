
// MoveBitFlip (moveData = 'int' (k))
using MoveBitFlip = FMove<int, ESolutionKP>;

// vizinhança: operador de "movimento" que faz "bit flip" (0 -> 1; 1 -> 0) na
// posição k
int fApplyFlip(const int& k, ESolutionKP& se) {
  // se.first[k] = 1 - se.first[k]; // inverte elemento 'k'
  se.first[k] = !se.first[k];  // inverte elemento 'k'
  return k;                    // movimento reverso
}

uptr<Move<ESolutionKP>> makeMoveBitFlip(int k) {
  return uptr<Move<ESolutionKP>>(new MoveBitFlip{k, fApplyFlip});
}