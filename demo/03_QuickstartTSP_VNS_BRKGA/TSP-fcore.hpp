#pragma once

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP =
    std::pair<std::vector<int>,  // first part of search space element: solution
                                 // (representation)
              Evaluation<int>    // second part of search space element:
                                 // evaluation (objective value)
              >;

// TSP problem context and data reads
class ProblemContext {
 public:
  int n{0};          // number of clients
  Matrix<int> dist;  // distance matrix (Euclidean)
  // load data from Scanner
  void load(Scanner& scanner) {
    n = *scanner.nextInt();    // reads number of clients
    dist = Matrix<int>(n, n);  // initializes n x n matrix
    //
    vector<double> xvalues(n);
    vector<double> yvalues(n);
    //
    for (int i = 0; i < n; i++) {
      scanner.next();
      xvalues[i] = *scanner.nextDouble();  // reads x
      yvalues[i] = *scanner.nextDouble();  // reads y
    }
    // calculate distance values, for every client pair (i,j)
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        dist(i, j) = (int)::round(distance(xvalues.at(i), yvalues.at(i),
                                           xvalues.at(j), yvalues.at(j)));
  }
  // euclidean distance (double as return)
  double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
};
// Create TSP Problem Context
// ProblemContext pTSP;

Evaluation<int> fevaluate(sref<ProblemContext> pTSP,
                          const std::vector<int>& s) {
  int f = 0;
  for (int i = 0; i < int(pTSP->n) - 1; i++) f += pTSP->dist(s[i], s[i + 1]);
  f += pTSP->dist(s[int(pTSP->n) - 1], s[0]);
  return Evaluation<int>{f};
}

// Evaluate
// FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE> eval{fevaluate};
std::vector<int> frandom(sref<ProblemContext> pTSP) {
  vector<int> v(pTSP->n, -1);  // get information from context
  for (int i = 0; i < (int)v.size(); i++) v[i] = i;
  std::random_shuffle(v.begin(), v.end());
  return v;
}

// Generate random solution
// FConstructive<std::vector<int>, ProblemContext> crand{frandom};

std::pair<int, int> fApplySwap(sref<ProblemContext>,
                               const std::pair<int, int>& moveData,
                               ESolutionTSP& se) {
  int i = moveData.first;
  int j = moveData.second;
  // perform swap of clients i and j
  int aux = se.first[j];
  se.first[j] = se.first[i];
  se.first[i] = aux;
  return std::pair<int, int>(j, i);  // return a reverse move ('undo' move)s
}

// Swap move
using MoveSwap = FMoveP<std::pair<int, int>, ESolutionTSP, ProblemContext>;

uptr<Move<ESolutionTSP>> makeMoveSwap(sref<ProblemContext> p, int i, int j) {
  return uptr<Move<ESolutionTSP>>(new MoveSwap{p, make_pair(i, j), fApplySwap});
}
uptr<Move<ESolutionTSP>> fRandomSwap(sref<ProblemContext> pTSP,
                                     const ESolutionTSP& se) {
  int i = rand() % pTSP->n;
  int j = i;
  while (j <= i) {
    i = rand() % pTSP->n;
    j = rand() % pTSP->n;
  }
  return uptr<Move<ESolutionTSP>>(makeMoveSwap(pTSP, i, j));
}

// Swap move (NS)
// FNS<ESolutionTSP> nsswap{fRandomSwap};

auto make_nsseq(sref<ProblemContext> p) {
  sref<FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>> nsseq2{
      new FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>{
          p,
          [](sref<ProblemContext> pTSP,
             const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
            int i = rand() % pTSP->n;
            int j = i;
            while (j <= i) {
              i = rand() % pTSP->n;
              j = rand() % pTSP->n;
            }
            return uptr<Move<ESolutionTSP>>(makeMoveSwap(pTSP, i, j));
          },
          // iterator initialization (fGenerator)
          [](sref<ProblemContext> pTSP, const ESolutionTSP& se)
              -> std::pair<int, int> { return make_pair(-1, -1); },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
            // void (*fFirst)(IMS&),                   // iterator.first()
            p.first = 0;
            p.second = 1;
          },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
            // void (*fNext)(IMS&),                    // iterator.next()
            if (p.second < (pTSP->n - 1))
              p.second++;
            else {
              p.first++;
              p.second = p.first + 1;
            }
          },
          [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> bool {
            // bool (*fIsDone)(IMS&),                  //
            // iterator.isDone()
            return p.first >= pTSP->n - 1;
          },
          [](sref<ProblemContext> pTSP,
             std::pair<int, int>& p) -> uptr<Move<ESolutionTSP>> {
            // uptr<Move<XES>> (*fCurrent)(IMS&)       //
            // iterator.current()
            return uptr<Move<ESolutionTSP>>(
                makeMoveSwap(pTSP, p.first, p.second));
          }  // FNSSeq
      }};
  return nsseq2;
}

class OptFrameDemoTSP {
 public:
  sref<ProblemContext> pTSP;
  sref<FConstructive<std::vector<int>, ProblemContext>> randomConstructive;
  sref<FEvaluator<ESolutionTSP, MINIMIZE, ProblemContext>> eval;
  sref<FNS<ESolutionTSP, ProblemContext>> nsSwap;
  sref<FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>> nsseqSwap;
  sptr<DecoderRandomKeys<ESolutionTSP, double>> decoder;

  explicit OptFrameDemoTSP(sref<ProblemContext> p)
      : pTSP{p},
        randomConstructive{
            new FConstructive<std::vector<int>, ProblemContext>{p, frandom}},
        eval{new FEvaluator<ESolutionTSP, MINIMIZE, ProblemContext>{p,
                                                                    fevaluate}},
        nsSwap{new FNS<ESolutionTSP, ProblemContext>{p, fRandomSwap}},
        nsseqSwap{make_nsseq(p)}

  {}
};

//
