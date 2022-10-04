#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/MultiEvaluator.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace BTSP_fcore {

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionBTSP = std::pair<
    // first part of search space element: solution (representation)
    std::vector<int>,
    // second part of search space element: multi-evaluation (objective value)
    MultiEvaluation<int>>;

// helper type for (single-obj) evaluator adapters
using ESolutionBTSPSingle = std::pair<
    typename ESolutionBTSP::first_type,
    typename ESolutionBTSP::second_type::XEv>;

/*
optframe::evgoal<Self>&&
   HasClone<Self>&&
         HasToString<Self>&&
            HasGetObj<Self>&&
               optframe::ostreamable<Self>&& requires(Self e) {
                  typename Self::objType;
               } &&
               
*/

hasUpdateDiff auto global_ev1 = MultiEvaluation<int>();

optframe::evgoal auto global_ev = MultiEvaluation<int>();

static_assert(optframe::evgoal<MultiEvaluation<int>>);

static_assert(XEvaluation<MultiEvaluation<int>>);

static_assert(XEvaluation<MultiEvaluation<int>>);
static_assert(XEvaluation<typename ESolutionBTSP::second_type>);
static_assert(XESolution<ESolutionBTSP>);

// TSP problem context and data reads
class ProblemContextBTSP {
 public:
  int n;              // number of clients
  Matrix<int> dist1;  // distance matrix (Euclidean)
  Matrix<int> dist2;  // distance matrix (Euclidean)
  // load data from Scanner
  void load(Scanner& scanner) {
    n = *scanner.nextInt();     // reads number of clients
    dist1 = Matrix<int>(n, n);  // initializes n x n matrix
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
        dist1(i, j) = ::round(distance(xvalues.at(i), yvalues.at(i),
                                       xvalues.at(j), yvalues.at(j)));
    dist2 = dist1;
  }
  // euclidean distance (double as return)
  double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
};
// Create TSP Problem Context
ProblemContextBTSP pBTSP;

Evaluation<int>
fevaluate0(const std::vector<int>& s) {
  int f1 = 0;
  for (int i = 0; i < int(pBTSP.n) - 1; i++)
    f1 += pBTSP.dist1(s[i], s[i + 1]);
  f1 += pBTSP.dist1(s[int(pBTSP.n) - 1], s[0]);
  return Evaluation<int>{f1};
}

Evaluation<int>
fevaluate1(const std::vector<int>& s) {
  int f2 = 0;
  for (int i = 0; i < int(pBTSP.n) - 1; i++)
    f2 += pBTSP.dist2(s[i], s[i + 1]);
  f2 += pBTSP.dist2(s[int(pBTSP.n) - 1], s[0]);
  return Evaluation<int>{f2};
}

// Evaluate
/*
sref<MultiEvaluator<typename ESolutionBTSP::first_type,
                    typename ESolutionBTSP::second_type>>
    ev{new FMultiEvaluator<ESolutionBTSP>{fevaluate}};
*/

sref<Evaluator<typename ESolutionBTSPSingle::first_type,
               typename ESolutionBTSPSingle::second_type>>
    ev0{
        new FEvaluator<ESolutionBTSPSingle, MinOrMax::MINIMIZE>{fevaluate0}};

sref<Evaluator<typename ESolutionBTSPSingle::first_type,
               typename ESolutionBTSPSingle::second_type>>
    ev1{
        new FEvaluator<ESolutionBTSPSingle, MinOrMax::MINIMIZE>{fevaluate1}};

vsref<Evaluator<typename ESolutionBTSPSingle::first_type,
                typename ESolutionBTSPSingle::second_type>>
    ev_list = {ev0, ev1};

sref<MultiEvaluator<ESolutionBTSP>>
    ev{new MultiEvaluator<ESolutionBTSP>(ev_list)};

// ===========================

std::vector<int>
frandom() {
  vector<int> v(pBTSP.n, -1);  // get information from context
  for (unsigned i = 0; i < v.size(); i++)
    v[i] = i;
  // leave 0 on first position and shuffle the rest
  std::random_shuffle(v.begin() + 1, v.end());
  return v;
}

// Generate random solution
FConstructive<std::vector<int>> crand{frandom};

//
class MoveSwap : public Move<ESolutionBTSP> {
 public:
  int i, j;

  MoveSwap(int _i, int _j)
      : i{_i}, j{_j} {
  }

  bool canBeApplied(const ESolutionBTSP& se) override {
    return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
  }

  /*
  uptr<Move<ESolutionBTSP>> applyUpdate(ESolutionBTSP& se) override {
    // input cannot be outdated
    assert(!se.second.isOutdated());
    auto& s = se.first;
    int diff = -pBTSP.dist(s[i - 1], s[i]) - pBTSP.dist(s[i], s[(i + 1) % pBTSP.n]) - pBTSP.dist(s[j - 1], s[j]) - pBTSP.dist(s[j], s[(j + 1) % pBTSP.n]);
    diff += pBTSP.dist(s[i - 1], s[j]) + pBTSP.dist(s[j], s[(i + 1) % pBTSP.n]) + pBTSP.dist(s[j - 1], s[i]) + pBTSP.dist(s[i], s[(j + 1) % pBTSP.n]);
    // solution swap
    auto rev = this->apply(se);
    se.second.setObjFunction(se.second.evaluation() + diff);
    return rev;
  }

  virtual op<Evaluation<int>> cost(const ESolutionBTSP& se, bool allowEstimated) override {
    assert(!se.second.isOutdated());
    auto& s = se.first;
    int diff = -pBTSP.dist(s[i - 1], s[i]) - pBTSP.dist(s[i], s[(i + 1) % pBTSP.n]) - pBTSP.dist(s[j - 1], s[j]) - pBTSP.dist(s[j], s[(j + 1) % pBTSP.n]);
    diff += pBTSP.dist(s[i - 1], s[j]) + pBTSP.dist(s[j], s[(i + 1) % pBTSP.n]) + pBTSP.dist(s[j - 1], s[i]) + pBTSP.dist(s[i], s[(j + 1) % pBTSP.n]);
    return std::make_optional(Evaluation<int>(diff));
  }
  */

  uptr<Move<ESolutionBTSP>> apply(ESolutionBTSP& se) override {
    // perform swap of clients i and j
    int aux = se.first[j];
    se.first[j] = se.first[i];
    se.first[i] = aux;
    return uptr<Move<ESolutionBTSP>>(new MoveSwap{j, i});  // return a reverse move ('undo' move)s
  }

  bool
  operator==(const Move<ESolutionBTSP>& other) const override {
    auto& fmove = (MoveSwap&)other;
    return (i == fmove.i) && (j == fmove.j);
  }
};

uptr<Move<ESolutionBTSP>>
makeMoveSwap(int i, int j) {
  return uptr<Move<ESolutionBTSP>>(new MoveSwap{i, j});
}

uptr<Move<ESolutionBTSP>>
fRandomSwap(const ESolutionBTSP& se) {
  int i = rand() % pBTSP.n;
  int j = i;
  while (j <= i) {
    i = rand() % pBTSP.n;
    j = rand() % pBTSP.n;
  }
  //return uptr<Move<ESolutionBTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap, fDefaultCanBeApplied<std::pair<int, int>, ESolutionBTSP>, fCompare });
  return uptr<Move<ESolutionBTSP>>(makeMoveSwap(i, j));
}

// Swap move (NS)
FNS<ESolutionBTSP> nsswap{fRandomSwap};

// Swap move (NSSeq) - with "Boring" iterator
FNSSeq<std::pair<int, int>, ESolutionBTSP> nsseq{
    [](const ESolutionBTSP& se) -> uptr<Move<ESolutionBTSP>> {
      int i = rand() % pBTSP.n;
      int j = i;
      while (j <= i) {
        i = rand() % pBTSP.n;
        j = rand() % pBTSP.n;
      }
      //return uptr<Move<ESolutionBTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap, fDefaultCanBeApplied<std::pair<int, int>, ESolutionBTSP>, fCompare });
      return uptr<Move<ESolutionBTSP>>(makeMoveSwap(i, j));
    },
    // iterator initialization (fGenerator)
    [](const ESolutionBTSP& se) -> std::pair<int, int> {
      return make_pair(-1, -1);
    },
    [](std::pair<int, int>& p) -> void {
      //void (*fFirst)(IMS&),                   // iterator.first()
      p.first = 0;
      p.second = 1;
    },
    [](std::pair<int, int>& p) -> void {
      //void (*fNext)(IMS&),                    // iterator.next()
      if (p.second < (pBTSP.n - 1))
        p.second++;
      else {
        p.first++;
        p.second = p.first + 1;
      }
    },
    [](std::pair<int, int>& p) -> bool {
      //bool (*fIsDone)(IMS&),                  // iterator.isDone()
      return p.first >= pBTSP.n - 1;
    },
    [](std::pair<int, int>& p) -> uptr<Move<ESolutionBTSP>> {
      //uptr<Move<XES>> (*fCurrent)(IMS&)       // iterator.current()
      //return uptr<Move<ESolutionBTSP>>(new MoveSwap{ p, fApplySwap, fDefaultCanBeApplied<std::pair<int, int>, ESolutionBTSP>, fCompare });
      return uptr<Move<ESolutionBTSP>>(makeMoveSwap(p.first, p.second));
    }};
//
sref<NSSeq<ESolutionBTSP>> nsseq2{
    new FNSSeq<std::pair<int, int>, ESolutionBTSP>{
        [](const ESolutionBTSP& se) -> uptr<Move<ESolutionBTSP>> {
          int i = rand() % pBTSP.n;
          int j = i;
          while (j <= i) {
            i = rand() % pBTSP.n;
            j = rand() % pBTSP.n;
          }
          //return uptr<Move<ESolutionBTSP>>(new MoveSwap{ make_pair(i, j), fApplySwap, fDefaultCanBeApplied<std::pair<int, int>, ESolutionBTSP>, fCompare });
          return uptr<Move<ESolutionBTSP>>(makeMoveSwap(i, j));
        },
        // iterator initialization (fGenerator)
        [](const ESolutionBTSP& se) -> std::pair<int, int> {
          return make_pair(-1, -1);
        },
        [](std::pair<int, int>& p) -> void {
          //void (*fFirst)(IMS&),                   // iterator.first()
          p.first = 0;
          p.second = 1;
        },
        [](std::pair<int, int>& p) -> void {
          //void (*fNext)(IMS&),                    // iterator.next()
          if (p.second < (pBTSP.n - 1))
            p.second++;
          else {
            p.first++;
            p.second = p.first + 1;
          }
        },
        [](std::pair<int, int>& p) -> bool {
          //bool (*fIsDone)(IMS&),                  // iterator.isDone()
          return p.first >= pBTSP.n - 1;
        },
        [](std::pair<int, int>& p) -> uptr<Move<ESolutionBTSP>> {
          //uptr<Move<XES>> (*fCurrent)(IMS&)       // iterator.current()
          //return uptr<Move<ESolutionBTSP>>(new MoveSwap{ p, fApplySwap, fDefaultCanBeApplied<std::pair<int, int>, ESolutionBTSP>, fCompare });
          return uptr<Move<ESolutionBTSP>>(makeMoveSwap(p.first, p.second));
        }}  // FNSSeq
};          // nsseq2

}  // namespace BTSP_fcore
