
std::pair<int, int>
fApplySwap(const std::pair<int, int>& moveData, ESolutionTSP& se)
{
   int i = moveData.first;
   int j = moveData.second;
   // perform swap of clients i and j
   int aux = se.first[j];
   se.first[j] = se.first[i];
   se.first[i] = aux;
   return std::pair<int, int>(j, i); // return a reverse move ('undo' move)s
}

// Swap move
using MoveSwap = FMove<std::pair<int, int>, ESolutionTSP>;

uptr<Move<ESolutionTSP>>
fRandomSwap(const ESolutionTSP& se)
{
   int i = rand() % pTSP.n;
   int j = i;
   while (j <= i) {
      i = rand() % pTSP.n;
      j = rand() % pTSP.n;
   }
   return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap });
}

// Swap move (NS)
FNS<ESolutionTSP> nsswap{
   fRandomSwap
};
