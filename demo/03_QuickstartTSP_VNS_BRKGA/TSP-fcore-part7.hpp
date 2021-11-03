
//
sref<NSSeq<ESolutionTSP>> nsseq2{
   new FNSSeq<std::pair<int, int>, ESolutionTSP>{
     [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
        int i = rand() % pTSP.n;
        int j = i;
        while (j <= i) {
           i = rand() % pTSP.n;
           j = rand() % pTSP.n;
        }
        return uptr<Move<ESolutionTSP>>(makeMoveSwap(i, j));
     },
     // iterator initialization (fGenerator)
     [](const ESolutionTSP& se) -> std::pair<int, int> {
        return make_pair(-1, -1);
     },
     [](std::pair<int, int>& p) -> void {
        //void (*fFirst)(IMS&),                   // iterator.first()
        p.first = 0;
        p.second = 1;
     },
     [](std::pair<int, int>& p) -> void {
        //void (*fNext)(IMS&),                    // iterator.next()
        if (p.second < (pTSP.n - 1))
           p.second++;
        else {
           p.first++;
           p.second = p.first + 1;
        }
     },
     [](std::pair<int, int>& p) -> bool {
        //bool (*fIsDone)(IMS&),                  // iterator.isDone()
        return p.first >= pTSP.n - 1;
     },
     [](std::pair<int, int>& p) -> uptr<Move<ESolutionTSP>> {
        //uptr<Move<XES>> (*fCurrent)(IMS&)       // iterator.current()
        return uptr<Move<ESolutionTSP>>(makeMoveSwap(p.first, p.second));
     } } // FNSSeq
};       // nsseq2
