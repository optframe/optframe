#pragma once

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
using namespace scannerpp;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace TSP_fcore {

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP1 = std::pair<
  std::vector<int>,  // first part of search space element: solution (representation)
  Evaluation<double> // second part of search space element: evaluation (objective value)
  >;

template<class X = nullptr_t>
using ESolutionTSP2 = ESolutionTSP1;

using ESolutionTSP = ESolutionTSP2<>;

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

Evaluation<double>
fevaluate(const std::vector<int>& s)
{
   double f = 0;
   for (int i = 0; i < int(pTSP.n) - 1; i++)
      f += pTSP.dist(s[i], s[i + 1]);
   f += pTSP.dist(s[int(pTSP.n) - 1], s[0]);
   return Evaluation<double>{ f };
}

// Evaluate
FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE>
  ev{
     fevaluate
  };

//sref<GeneralEvaluator<ESolutionTSP>> ev2{
//   new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE>{
//     fevaluate }
//};

// ===========================

std::vector<int>
frandom()
{
   vector<int> v(pTSP.n, -1); // get information from context
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = i;
   std::random_shuffle(v.begin(), v.end());
   return v;
}

// Generate random solution
FConstructive<std::vector<int>> crand{
   frandom
};

std::pair<int, int>
fApplySwap(const std::pair<int, int>& moveData, ESolutionTSP& se)
{
   int i = moveData.first;
   int j = moveData.second;
   // perform swap of clients i and j
   int aux = se.first[j];
   se.first[j] = se.first[i];
   se.first[i] = aux;
   return std::pair<int, int>(j, i); // return a reverse move ('undo' move)s
}

// Swap move
using MoveSwap = FMove<std::pair<int, int>, ESolutionTSP>;

uptr<Move<ESolutionTSP>>
fRandomSwap(const ESolutionTSP& se)
{
   int i = rand() % pTSP.n;
   int j = i;
   while (j <= i) {
      i = rand() % pTSP.n;
      j = rand() % pTSP.n;
   }
   return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap });
}

// Swap move (NS)
FNS<ESolutionTSP> nsswap{
   fRandomSwap
};

// Swap move (NSSeq) - with "Boring" iterator
FNSSeq<std::pair<int, int>, ESolutionTSP> nsseq{
   [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
      int i = rand() % pTSP.n;
      int j = i;
      while (j <= i) {
         i = rand() % pTSP.n;
         j = rand() % pTSP.n;
      }
      return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap });
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
      return uptr<Move<ESolutionTSP>>(new MoveSwap{ p, fApplySwap });
   }
};
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
        return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap });
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
        return uptr<Move<ESolutionTSP>>(new MoveSwap{ p, fApplySwap });
     } } // FNSSeq
};       // nsseq2

} // TSP_fcore
