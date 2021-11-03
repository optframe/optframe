class MoveSwap : public Move<ESolutionTSP>
{
public:
   int i, j;

   MoveSwap(int _i, int _j)
     : i{ _i }
     , j{ _j }
   {
   }

   bool canBeApplied(const ESolutionTSP& se) override
   {
      return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
   }

   uptr<Move<ESolutionTSP>> applyUpdate(ESolutionTSP& se) override
   {
      // input cannot be outdated
      assert(!se.second.outdated);
      auto& s = se.first;
      int diff = -pTSP.dist(s[i - 1], s[i]) - pTSP.dist(s[i], s[(i + 1) % pTSP.n]) - pTSP.dist(s[j - 1], s[j]) - pTSP.dist(s[j], s[(j + 1) % pTSP.n]);
      diff += pTSP.dist(s[i - 1], s[j]) + pTSP.dist(s[j], s[(i + 1) % pTSP.n]) + pTSP.dist(s[j - 1], s[i]) + pTSP.dist(s[i], s[(j + 1) % pTSP.n]);
      // solution swap
      auto rev = this->apply(se);
      se.second.setObjFunction(se.second.evaluation() + diff);
      return rev;
   }

   uptr<Move<ESolutionTSP>> apply(ESolutionTSP& se) override
   {
      // perform swap of clients i and j
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;
      return uptr<Move<ESolutionTSP>>(new MoveSwap{ j, i }); // return a reverse move ('undo' move)s
   }

   virtual op<Evaluation<int>> cost(const ESolutionTSP& se, bool allowEstimated) override
   {
      assert(!se.second.outdated);
      auto& s = se.first;
      int diff = -pTSP.dist(s[i - 1], s[i]) - pTSP.dist(s[i], s[(i + 1) % pTSP.n]) - pTSP.dist(s[j - 1], s[j]) - pTSP.dist(s[j], s[(j + 1) % pTSP.n]);
      diff += pTSP.dist(s[i - 1], s[j]) + pTSP.dist(s[j], s[(i + 1) % pTSP.n]) + pTSP.dist(s[j - 1], s[i]) + pTSP.dist(s[i], s[(j + 1) % pTSP.n]);
      return std::make_optional(Evaluation<int>(diff));
   }

   bool
   operator==(const Move<ESolutionTSP>& other) const override
   {
      auto& fmove = (MoveSwap&)other;
      return (i == fmove.i) && (j == fmove.j);
   }
};

uptr<Move<ESolutionTSP>>
makeMoveSwap(int i, int j)
{
   return uptr<Move<ESolutionTSP>>(new MoveSwap{ i, j });
}