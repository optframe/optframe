
class MyRandomKeysInitPop : public InitialPopulation<std::pair<std::vector<double>, Evaluation<int>>>
{
   using RSK = std::vector<double>;

private:
   int sz;
   sref<RandGen> rg;

public:
   MyRandomKeysInitPop(int size, sref<RandGen> _rg = new RandGen)
     : sz{ size }
     , rg{ _rg }
   {
   }

   // copy constructor
   MyRandomKeysInitPop(const MyRandomKeysInitPop& self)
     : sz{ self.sz }
     , rg{ self.rg }
   {
   }

   Population<std::pair<RSK, Evaluation<int>>> generatePopulation(unsigned populationSize, double timelimit) override
   {
      Population<std::pair<RSK, Evaluation<int>>> pop;

      for (unsigned i = 0; i < populationSize; i++) {
         vector<double>* d = new vector<double>(sz);
         for (int j = 0; j < sz; j++)
            d->at(j) = (rg->rand() % 100000) / 100000.0;
         pop.push_back(d);
      }

      return pop;
   }
};