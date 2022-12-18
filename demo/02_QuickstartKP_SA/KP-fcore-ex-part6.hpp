
// gerador de movimentos aleatórios do tipo BitFlip
uptr<Move<ESolutionKP>> fRandomFlip(sref<ProblemContext> pKP,
                                    const ESolutionKP& se) {
  int k = ::rand() % pKP->n;  // sorteia um item entre [0..n-1]
  // cria um "movimento" de flip, na posição k, com operação 'fApplyFlip'
  return uptr<Move<ESolutionKP>>(makeMoveBitFlip(k));
}

// Estrutura de Vizinhança para BitFlip (NS)
// FNS<ESolutionKP> nsFlip{fRandomFlip};

// ================== EVERYTHING TOGETHER ====================

class OptFrameDemoKP {
 public:
  FConstructive<std::vector<bool>, ProblemContext> randomConstructive;
  FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext> evalKP;
  FNS<ESolutionKP, ProblemContext> nsFlip;

  explicit OptFrameDemoKP(sref<ProblemContext> p)
      : randomConstructive{p, frandom},
        evalKP{FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext>{p, fevaluate}},
        nsFlip{FNS<ESolutionKP, ProblemContext>{p, fRandomFlip}} {}
};
