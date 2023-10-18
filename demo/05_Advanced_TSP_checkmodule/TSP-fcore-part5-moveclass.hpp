class MoveSwap : public Move<ESolutionTSP> {
 public:
  int i, j;

  MoveSwap(int _i, int _j) : i{_i}, j{_j} {}

  bool canBeApplied(const ESolutionTSP& se) override {
    return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
  }

  uptr<Move<ESolutionTSP>> apply(ESolutionTSP& se) override {
    // perform swap of clients i and j
    int aux = se.first[j];
    se.first[j] = se.first[i];
    se.first[i] = aux;
    return uptr<Move<ESolutionTSP>>(
        new MoveSwap{j, i});  // return a reverse move ('undo' move)s
  }

  bool operator==(const Move<ESolutionTSP>& other) const override {
    auto& fmove = (MoveSwap&)other;
    return (i == fmove.i) && (j == fmove.j);
  }
};

uptr<Move<ESolutionTSP>> makeMoveSwap(int i, int j) {
  return uptr<Move<ESolutionTSP>>(new MoveSwap{i, j});
}