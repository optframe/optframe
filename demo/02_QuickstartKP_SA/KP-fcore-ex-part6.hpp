
// gerador de movimentos aleatórios do tipo BitFlip
uptr<Move<ESolutionKP>>
fRandomFlip(const ESolutionKP& se)
{
   int k = ::rand() % pKP.n; // sorteia um item entre [0..n-1]
   // cria um "movimento" de flip, na posição k, com operação 'fApplyFlip'
   return uptr<Move<ESolutionKP>>(makeMoveBitFlip(k));
}

// Estrutura de Vizinhança para BitFlip (NS)
FNS<ESolutionKP> nsFlip{ fRandomFlip };
