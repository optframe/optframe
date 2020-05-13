#include <algorithm>
#include <functional>
#include <iostream>

#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/Util/printable.h>

using namespace std;
using namespace optframe;

using ESolutionTSP = std::pair<
  std::vector<int>,  // first part of search space element: solution (representation)
  Evaluation<double> // second part of search space element: evaluation (objective value)
  >;

constexpr int TSP_N = 3;

// Generate random solution
using TSPRandom = FConstructive <
                  std::vector<int>,
      [](double timelimit) -> auto
{
   vector<int> v(TSP_N, -1); // get information from context
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = i;
   std::random_shuffle(v.begin(), v.end());
   return make_optional(v);
}
> ;

// Swap move
using MoveSwap = FMove<
  std::pair<int, int>,
  ESolutionTSP,
  [](const std::pair<int, int>& moveData, ESolutionTSP& se) -> std::pair<int, int> {
     int i = moveData.first;
     int j = moveData.second;
     // perform swap of clients i and j
     int aux = se.first[j];
     se.first[j] = se.first[i];
     se.first[i] = aux;
     return std::pair<int, int>(j, i); // return a reverse move ('undo' move)
  }>;

// Swap move (NSSeq) - with "Fancy" iterator (coroutines)
using NSSeqSwapFancy = FNSSeqFancy<
  ESolutionTSP,
  [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
     int i = rand() % TSP_N;
     int j = i;
     while (j <= i) {
        i = rand() % TSP_N;
        j = rand() % TSP_N;
     }
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  [](const ESolutionTSP& se) -> Generator<Move<ESolutionTSP>*> {
     for (int i = 0; i < int(TSP_N) - 1; i++)
        for (int j = i + 1; j < TSP_N; j++)
           co_yield new MoveSwap{ make_pair(i, j) }; // implicit unique_ptr requirements
  }>;

int
main()
{
   // create simple solution
   TSPRandom crand;
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   ESolutionTSP esol(sol, Evaluation<double>(1.0));
   esol.second.print(); // print evaluation

   std::cout << std::endl;
   std::cout << "begin listing NSSeqSwapFancy" << std::endl;
   NSSeqSwapFancy swapFancy;
   auto it1 = swapFancy.getIterator(esol);
   for (it1->first(); !it1->isDone(); it1->next())
      it1->current()->print();
   std::cout << "end listing NSSeqSwapFancy" << std::endl;

   std::cout << "FINISHED" << std::endl;
   return 0;
}