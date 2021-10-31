

#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/EvolutionaryAlgorithms/BRKGA.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/InitialPopulation.h>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
//using namespace TSP_fcore;

class MyRandomKeysInitPop : public InitialPopulation<std::pair<std::vector<double>, Evaluation<double>>>
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

   Population<std::pair<RSK, Evaluation<double>>> generatePopulation(unsigned populationSize, double timelimit) override
   {
      Population<std::pair<RSK, Evaluation<double>>> pop;

      for (unsigned i = 0; i < populationSize; i++) {
         vector<double>* d = new vector<double>(sz);
         for (int j = 0; j < sz; j++)
            d->at(j) = (rg->rand() % 100000) / 100000.0;
         pop.push_back(d);
      }

      return pop;
   }
};
pair<Evaluation<double>, vector<int>>
fDecode(const vector<double>& rk)
{
   vector<pair<double, int>> v(rk.size());
   int k = 0;
   for (unsigned i = 0; i < v.size(); i++)
      v[k] = pair<double, int>(rk[i], i);

   sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
      return i.first < j.first;
   });

   // R = vector<int>
   vector<int> p(v.size());
   for (unsigned i = 0; i < v.size(); i++)
      p[i] = v[i].second;

   Evaluation<double> e = eval.evaluate(p);
   return make_pair(e, p);
}

// evaluator random keys (for TSP)
FDecoderRK<std::vector<int>, Evaluation<>, double, MinOrMax::MINIMIZE> decoder{
   fDecode
};
int
main()
{
   sref<RandGen> rg = new RandGen; // avoids weird windows OS interactions

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // Parameters BRKGA
   // (C1): Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :

   sref<DecoderRandomKeys<ESolutionTSP::first_type, ESolutionTSP::second_type, double>> _decoder = decoder;
   sref<InitialPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>> _initPop = new MyRandomKeysInitPop(pTSP.n); // passing key_size

   //eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
   BRKGA<ESolutionTSP, double> brkga(
     _decoder,
     MyRandomKeysInitPop(pTSP.n, rg), // key_size = pTSP.n
     30,
     1000,
     0.4,
     0.3,
     0.6,
     rg);

   auto searchOut = brkga.search(3.0); // 3.0 seconds max
   ESolutionTSP best = *searchOut.best;
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}
