#pragma once

#include <algorithm>
#include <functional>
#include <iostream>

#include <OptFCore/FxCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

//
#include <OptFCore/FxRNSSeq.hpp> // magic iterators with random coupled with regular iterator
// structured randomized iterators

#include <OptFCore/FNSEnum.hpp>
#include <OptFCore/FxNSEnum.hpp>
#include <OptFCore/FxRNSEnum.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace TSP_fxcore {

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
using TSPEval = FxEvaluator <
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
using TSPRandom = FxConstructive<
  std::vector<int>,
  [](double timelimit) -> std::optional<std::vector<int>> {
     vector<int> v(pTSP.n, -1); // get information from context
     for (unsigned i = 0; i < v.size(); i++)
        v[i] = i;
     std::random_shuffle(v.begin(), v.end());
     return make_optional(v);
  }>;

// Swap move
using MoveSwap = FxMove<
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
using NSSwap = FxNS<
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
using NSSeqSwapBoring = FxNSSeqBoring<
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
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ p });
  }>;

// Swap move (NSSeq) - with "Fancy" iterator (coroutines)
using NSSeqSwapFancy = FxNSSeqFancy<
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
  [](const ESolutionTSP& se) -> Generator<Move<ESolutionTSP>*> {
     for (int i = 0; i < int(pTSP.n) - 1; i++)
        for (int j = i + 1; j < pTSP.n; j++)
           co_yield new MoveSwap{ make_pair(i, j) }; // implicit unique_ptr requirements
  }>;

// Swap move (RNSSeq) - Random + Iterator + coroutines
using RNSSeqSwapV1 = FxRNSSeq<
  ESolutionTSP,
  int, // no cache
  [](const ESolutionTSP& se,
     int& nocache,
     sref<RandGen> rg,
     bool single) -> Generator<Move<ESolutionTSP>*> {
     // optimistic cache: count moves and allow repetitions, also missing moves
     //
     // always i < j
     int i = rg->rand() % int(pTSP.n - 1);
     int j = rg->rand() % int(pTSP.n - i - 1) + i + 1;
     // if single move, game is over
     if (single) {
        co_yield new MoveSwap{ make_pair(i, j) };
        co_return; // STOP
     } else {
        // note that this will reset cache!
        // one cannot run random moves at the same time an active iterator is running!
        // if this is important, we can fix... this is a first version prototype
        nocache = 0;
        // this is a real iterator
        //int MAX_MOVES = pTSP.n * pTSP.n; // N² moves
        int MAX_MOVES = (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
        //
        while (nocache < MAX_MOVES) {
           nocache++;
           co_yield new MoveSwap{ make_pair(i, j) }; // implicit unique_ptr requirements
           // next move
           i = rg->rand() % int(pTSP.n - 1);
           j = rg->rand() % int(pTSP.n - i - 1) + i + 1;
        }
     }
  }>;

// Swap move (RNSSeq) - Random + Iterator + coroutines
using RNSSeqSwap = FxRNSSeq<
  ESolutionTSP,
  int, // no cache
  [](const ESolutionTSP& se,
     int& nocache,
     sref<RandGen> rg,
     bool single) -> Generator<Move<ESolutionTSP>*> {
     // optimistic cache: count moves, allow repetitions with missing moves
     // final result is "Assumed Local Optimum" (not "definitive local optimum")
     nocache = 0;
     int MAX_MOVES = (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
     while (nocache < MAX_MOVES) {
        // next move
        int i = rg->rand(0, pTSP.n - 2);
        int j = rg->rand(i + 1, pTSP.n - 1);
        co_yield new MoveSwap{ make_pair(i, j) };
        nocache++;
     }
  }>;

using RNSEnumSwap = FxRNSEnum<
  ESolutionTSP,
  []() -> unsigned int {
     return (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
  },
  [](unsigned int k)
    -> uptr<Move<ESolutionTSP>> {
     int i = k % (pTSP.n - 1);
     int j = k / (pTSP.n - 1) + i + 1;
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  }>;

using RNSEnumSwapShuffle = FxRNSEnum<
  ESolutionTSP,
  []() -> unsigned int {
     return (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
  },
  [](unsigned int k)
    -> uptr<Move<ESolutionTSP>> {
     int i = k % (pTSP.n - 1);
     int j = k / (pTSP.n - 1) + i + 1;
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  RNSEnumIteratorShuffle<ESolutionTSP>>;

using NSEnumSwap = FxNSEnum<
  ESolutionTSP,
  []() -> unsigned int {
     return (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
  },
  [](unsigned int k)
    -> uptr<Move<ESolutionTSP>> {
     int i = k % (pTSP.n - 1);
     int j = k / (pTSP.n - 1) + i + 1;
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  }>;

sref<RandGen> rg_test = new RandGen();
FNSEnum<ESolutionTSP>
fenum_swap(
  []() -> unsigned int {
     return (pTSP.n * (pTSP.n - 1)) / 2; // O(N²) moves
  },
  [](unsigned int k)
    -> uptr<Move<ESolutionTSP>> {
     int i = k % (pTSP.n - 1);
     int j = k / (pTSP.n - 1) + i + 1;
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  rg_test);

// =======
} // namespace TSP_fxcore
