
auto make_nsseq(sref<ProblemContext> p) {
  sref<FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>> nsseq2{
      new FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>{
          p,
          [](sref<ProblemContext> pTSP,
             const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
            int i = rand() % pTSP->n;
            int j = i;
            while (j <= i) {
              i = rand() % pTSP->n;
              j = rand() % pTSP->n;
            }
            return uptr<Move<ESolutionTSP>>(makeMoveSwap(pTSP, i, j));
          },
          // iterator initialization (fGenerator)
          [](sref<ProblemContext> pTSP, const ESolutionTSP& se)
              -> std::pair<int, int> { return make_pair(-1, -1); },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
            // void (*fFirst)(IMS&),                   // iterator.first()
            p.first = 0;
            p.second = 1;
          },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
            // void (*fNext)(IMS&),                    // iterator.next()
            if (p.second < (pTSP->n - 1))
              p.second++;
            else {
              p.first++;
              p.second = p.first + 1;
            }
          },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> bool {
            // bool (*fIsDone)(IMS&),                  //
            // iterator.isDone()
            return p.first >= pTSP->n - 1;
          },
          [](sref<ProblemContext> pTSP,
             std::pair<int, int>& p) -> uptr<Move<ESolutionTSP>> {
            // uptr<Move<XES>> (*fCurrent)(IMS&)       //
            // iterator.current()
            return uptr<Move<ESolutionTSP>>(
                makeMoveSwap(pTSP, p.first, p.second));
          }  // FNSSeq
      }};
  return nsseq2;
}

class OptFrameDemoTSP {
 public:
  sref<ProblemContext> pTSP;
  sref<FConstructive<std::vector<int>, ProblemContext>> randomConstructive;
  sref<FEvaluator<ESolutionTSP, MINIMIZE, ProblemContext>> eval;
  sref<FNS<ESolutionTSP, ProblemContext>> nsSwap;
  sref<FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>> nsseqSwap;
  sptr<DecoderRandomKeys<ESolutionTSP, double>> decoder;

  explicit OptFrameDemoTSP(sref<ProblemContext> p)
      : pTSP{p},
        randomConstructive{
            new FConstructive<std::vector<int>, ProblemContext>{p, frandom}},
        eval{new FEvaluator<ESolutionTSP, MINIMIZE, ProblemContext>{p,
                                                                    fevaluate}},
        nsSwap{new FNS<ESolutionTSP, ProblemContext>{p, fRandomSwap}},
        nsseqSwap{make_nsseq(p)}

  {}
};

//
