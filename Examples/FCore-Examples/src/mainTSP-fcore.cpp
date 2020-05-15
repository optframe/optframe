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
using namespace scannerpp;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace TSP_fcore {

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP = std::pair<
  std::vector<int>,  // first part of search space element: solution (representation)
  Evaluation<double> // second part of search space element: evaluation (objective value)
  >;

// TSP problem context and data reads
class ProblemContext
{
public:
   int n;               // number of clients
   Matrix<double> dist; // distance matrix (Euclidean)
   // load data from Scanner
   void load(Scanner& scanner)
   {
      n = *scanner.nextInt();      // reads number of clients
      dist = Matrix<double>(n, n); // initializes n x n matrix
      //
      vector<double> xvalues(n);
      vector<double> yvalues(n);
      //
      for (int i = 0; i < n; i++) {
         scanner.next();
         xvalues[i] = *scanner.nextDouble(); // reads x
         yvalues[i] = *scanner.nextDouble(); // reads y
      }
      // calculate distance values, for every client pair (i,j)
      for (int i = 0; i < n; i++)
         for (int j = 0; j < n; j++)
            dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j), yvalues.at(j));
   }
   // euclidean distance (double as return)
   double distance(double x1, double y1, double x2, double y2)
   {
      return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
   }
};
// Create TSP Problem Context
ProblemContext pTSP;

// Evaluate
using TSPEval = FEvaluator <
                ESolutionTSP,
      true, // minimization: true
  [](auto& s) -> auto
{
   double f = 0;
   for (int i = 0; i < int(pTSP.n) - 1; i++)
      f += pTSP.dist(s[i], s[i + 1]);
   f += pTSP.dist(s[int(pTSP.n) - 1], s[0]);
   return Evaluation<double>{ f };
}
> ;

// ===========================

// Generate random solution
using TSPRandom = FConstructive <
                  std::vector<int>,
      [](double timelimit) -> std::optional<std::vector<int>>
{
   vector<int> v(pTSP.n, -1); // get information from context
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

// Swap move (NS)
using NSSwap = FNS<
  ESolutionTSP,
  [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
     int i = rand() % pTSP.n;
     int j = i;
     while (j <= i) {
        i = rand() % pTSP.n;
        j = rand() % pTSP.n;
     }
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  }>;


// Swap move (NSSeq) - with "Boring" iterator
using NSSeqSwapBoring = FNSSeqBoring<
  std::pair<int, int>, // IMS (iterator memory)
  ESolutionTSP,
  [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
     int i = rand() % pTSP.n;
     int j = i;
     while (j <= i) {
        i = rand() % pTSP.n;
        j = rand() % pTSP.n;
     }
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  [](const ESolutionTSP& se) -> std::pair<int, int> {
     return make_pair(-1, -1);
  },
  [](std::pair<int, int>& p) -> void {
     //void (*fFirst)(IMS&),                   // iterator.first()
     /*
     if (n >= 2)
		{
			p1 = 0;
			p2 = 1;
			m = uptr<MOVE>(new MOVE(p1, p2, p));
		}
		else
			m = nullptr;
         */
  },
  [](std::pair<int, int>& p) -> void {
     //void (*fNext)(IMS&),                    // iterator.next()
  },
  [](std::pair<int, int>& p) -> bool {
     //bool (*fIsDone)(IMS&),                  // iterator.isDone()
     return true;
  },
  [](std::pair<int, int>& p) -> uptr<Move<ESolutionTSP>> {
     //uptr<Move<XES>> (*fCurrent)(IMS&)       // iterator.current()
     return nullptr;
  }>;

// Swap move (NSSeq) - with "Fancy" iterator (coroutines)
using NSSeqSwapFancy = FNSSeqFancy<
  ESolutionTSP,
  [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
     int i = rand() % pTSP.n;
     int j = i;
     while (j <= i) {
        i = rand() % pTSP.n;
        j = rand() % pTSP.n;
     }
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  [](const ESolutionTSP& se) -> Generator< Move<ESolutionTSP>* > {
     for (int i = 0; i < int(pTSP.n) - 1; i++)
        for (int j = i + 1; j < pTSP.n; j++)
           co_yield new MoveSwap{ make_pair(i, j) }; // implicit unique_ptr requirements
  }>;

} // TSP_fcore

// import everything on main()
using namespace TSP_fcore;

int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // evaluator
   TSPEval ev;

   // create simple solution
   TSPRandom crand;
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, ev.evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1) };
   move.print();

   NSSwap nsswap;
   // move for solution 'esol'
   auto m1 = nsswap.randomMove(esol);
   m1->print();

   std::cout << std::endl;
   std::cout << "begin listing NSSeqSwapFancy" << std::endl;
   NSSeqSwapFancy swapFancy;
   auto it1 = swapFancy.getIterator(esol);
   for (it1->first(); !it1->isDone(); it1->next())
      it1->current()->print();
   std::cout << "end listing NSSeqSwapFancy" << std::endl;

   // Random number generator
   RandGen rg;
   // testing simulated annealing
   BasicInitialSearch<ESolutionTSP> initRand(crand, ev);
   BasicSimulatedAnnealing<ESolutionTSP> sa{
      ev, initRand, swapFancy, 0.98, 100, 99999, rg
   };
   SearchStatus status = sa.search(StopCriteria<ESolutionTSP::second_type>{ 10.0 }); // 10.0 seconds max
   ESolutionTSP best = *sa.getBestSolution();
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}