
#include <iostream>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/PSO.hpp>
#include <algorithm>
#include <functional>
#include <iostream>

#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.h>

using namespace std;
using namespace optframe;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace example_pso {

using ESolutionPSO = std::pair<
  std::vector<double>, // first part of search space element: solution (representation)
  Evaluation<double>   // second part of search space element: evaluation (objective value)
  >;

Evaluation<double>
fevaluatePSO(const std::vector<double>& s)
{
   double f = 0;
   // TODO: calcular erros em relação a solução corrente 's'
   // (x-10)^2
   f = (s[0] - 10) * (s[0] - 10);
   return Evaluation<double>{ f };
}

// Evaluate
FEvaluator<ESolutionPSO, MinOrMax::MINIMIZE> evPSO{
   fevaluatePSO
};

//
} // example_pso

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace example_pso;

RandGen myRandGen; // good random generator

int
main()
{
   int seed = 1000012345;
   seed = time(NULL);

   // Particle Swarm Optimization
   // PSO(Evaluator<S, XEv, XES>& evaluator, unsigned pop_size, unsigned iter_max, vector<double> cI, vector<double> cS, RandGen& _rg)

   myRandGen.setSeed(seed);
   int nParam = 1; // param size

   vector<double> cI(nParam, -100.0);
   vector<double> cS(nParam, 100.0);

   PSO<> myPSO{
      evPSO,
      10, // pop_size
      50, // iter_max
      cI, // number of parameters is extracted from cI.size()
      cS,
      myRandGen
   };
   myPSO.setVerboseR();

   auto statusPSO = myPSO.search(5.0); // 5.0 seconds max

   std::cout << "BEST = " << statusPSO.best->second.evaluation()
             << " sol: " << statusPSO.best->first << std::endl;

   return 0;
}