
class MyRandomKeysInitEPop
    : public InitialEPopulation<
          std::pair<std::vector<double>, Evaluation<int>>> {
  using RSK = std::vector<double>;

 private:
  int sz;
  sref<RandGen> rg;

 public:
  explicit MyRandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
      : sz{size}, rg{_rg} {}

  // copy constructor
  // MyRandomKeysInitEPop(const MyRandomKeysInitEPop& self)
  //     : sz{self.sz}, rg{self.rg} {}

  // this generator cannot evaluate solutions
  bool canEvaluate() const override { return false; }

  VEPopulation<std::pair<RSK, Evaluation<int>>> generateEPopulation(
      unsigned populationSize, double timelimit) override {
    VEPopulation<std::pair<RSK, Evaluation<int>>> pop;

    for (unsigned i = 0; i < populationSize; i++) {
      vector<double> vd(sz);
      for (int j = 0; j < sz; j++) vd[j] = (rg->rand() % 100000) / 100000.0;
      // assert(!this->canEvaluate());
      std::pair<RSK, Evaluation<int>> ind{vd, Evaluation<int>{}};
      pop.push_back(ind);
    }

    return pop;
  }
};
