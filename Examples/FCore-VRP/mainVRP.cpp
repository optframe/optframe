// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// C++
#include <algorithm>
#include <functional>
#include <iostream>
//

#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/EA/PSO.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRP2Opt.hpp>
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRPExchange.hpp>
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRPOrOpt1.hpp>
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRPOrOpt2.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace example_VRP_RepADS {

// VRP problem context and data reads
class ProblemContext {
 public:
  int n{-1};            // number of clients
  int N{0};             // number of clients + depot
  Matrix<double> dist;  // distance matrix (Euclidean)
  vector<int> d;        // delivery packet size for each client
  int Q{0};             // homogeneous capacity among vehicles
  //
  // load data from Scanner
  void load(Scanner& scanner) {
    n = *scanner.nextInt();  // reads number of clients
    N = n + 1;
    dist = Matrix<double>(N, N);  // initializes N x N matrix
    //
    vector<double> xvalues(N);
    vector<double> yvalues(N);
    //
    for (int i = 0; i < N; i++) {
      scanner.next();
      xvalues[i] = *scanner.nextDouble();  // reads x
      yvalues[i] = *scanner.nextDouble();  // reads y
    }
    // calculate distance values, for every client pair (i,j)
    for (int i = 0; i < N; i++)
      for (int j = 0; j < N; j++)
        dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j),
                              yvalues.at(j));
    // reads homogeneous capacity Q
    Q = *scanner.nextInt();
    std::cout << "Q=" << Q << std::endl;
    // reads demands (depot 0 has no demands)
    d = vector<int>(N, 0);
    for (int i = 1; i < N; i++) {
      d[i] = *scanner.nextInt();  // reads d (demands/delivery size)
    }
  }
  // euclidean distance (double as return)
  double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
};
// Create VRP Problem Context
// ProblemContext pVRP;

// Representation + ADS
struct VRP_RepADS {
  vector<vector<int>> sol;  // solution ("route")
  // ADS part
  vector<int> ar;  // excess per route

  friend ostream& operator<<(ostream& os, const VRP_RepADS& self) {
    os << self.sol;
    return os;
  }
};

using ESolutionVRP =
    std::pair<VRP_RepADS,  // first part of search space element: solution
                           // (representation + ADS)
              Evaluation<double>  // second part of search space element:
                                  // evaluation (objective value)
              >;

// full evaluation: count distances (cannot update ADS, since its const)
Evaluation<double> fevaluateVRP(sref<ProblemContext> pVRP,
                                const VRP_RepADS& s) {
  double f = 0;                         // sumDistances
  constexpr double W_ExtraCap = 99999;  // high weight for extra cap
  double f_extra = 0;
  for (unsigned r = 0; r < s.sol.size(); r++) {
    int sumDemands = 0;  // sum demands in route
    for (int i = 0; i < ((int)s.sol[r].size()) - 1; i++) {
      f += pVRP->dist(s.sol[r][i], s.sol[r][i + 1]);
      sumDemands += pVRP->d[s.sol[r][i]];
    }
    f += pVRP->dist(s.sol[r][s.sol[r].size() - 1], 0);  // return to depot
    sumDemands += pVRP->d[s.sol[r][s.sol[r].size() - 1]];
    if (sumDemands > pVRP->Q) {
      f_extra += (sumDemands - pVRP->Q);
    }
  }

  return Evaluation<double>{f + W_ExtraCap * f_extra};
}

// Evaluate
// FEvaluator<ESolutionVRP, MinOrMax::MINIMIZE> demo.evalVRP{fevaluateVRP};

// random constructive
VRP_RepADS frandom(sref<ProblemContext> pVRP) {
  std::cout << "frandom(n=" << pVRP->n << ")" << std::endl;
  VRP_RepADS rep;

  // shuffle all clients (client 0 is 'depot')
  vector<int> v(pVRP->n, -1);  // get information from context
  for (int i = 0; i < v.size(); i++) v[i] = i + 1;  // excludes depot 0
  // shuffle (TODO: use randgen)
  std::random_shuffle(v.begin(), v.end());
  //
  // begin random construction
  // clients are added one by one, unless capacity is exceeded, and a new route
  // is formed
  int sumDemands =
      9999999;  // accumulate demands on current route (starts VERY BIG)
  //
  for (unsigned i = 0; i < v.size(); i++) {
    if (sumDemands + pVRP->d[v[i]] > pVRP->Q) {
      // start new route
      rep.sol.push_back(
          vector<int>(1, 0));  // "empty route" (depot 0 is always present)
      sumDemands = 0;
    }
    // add client at position v[i]
    rep.sol[rep.sol.size() - 1].push_back(v[i]);
    // add demands
    sumDemands += pVRP->d[v[i]];
  }
  // feasible (but not very good) route is done
  return rep;
}

// Generate random solution
// FConstructive<VRP_RepADS> crand{frandom};

//
}  // namespace example_VRP_RepADS

// import everything on main()
using namespace std;                 // NOLINT
using namespace optframe;            // NOLINT
using namespace scannerpp;           // NOLINT
using namespace example_VRP_RepADS;  // NOLINT

vector<vector<int>>& localGetRoutes(const ESolutionVRP& s) {
  vector<vector<int>>& v =
      const_cast<vector<vector<int>>&>(s.first.sol);  // NOLINT
  return v;
}

class OptFrameCoreVRP {
 public:
  sref<FConstructive<VRP_RepADS, ProblemContext>> randomConstructive;
  sref<FEvaluator<ESolutionVRP, MINIMIZE, ProblemContext>> evalVRP;

  explicit OptFrameCoreVRP(sref<ProblemContext> p)
      : randomConstructive{new FConstructive<VRP_RepADS, ProblemContext>{
            p, frandom}},
        evalVRP{new FEvaluator<ESolutionVRP, MINIMIZE, ProblemContext>{
            p, fevaluateVRP}}

  {}
};

int main() {
  std::cout << "VRP FCore example" << std::endl;
  sref<RandGen> myRandGen = RandGen{};  // good random generator
  int64_t seed = 1000012345;
  seed = time(NULL);

  myRandGen->setSeed(seed);

  // load instance
  std::cout << "loading 'instance10.txt'" << std::endl;
  Scanner scanner{File("instance10.txt")};

  sref<ProblemContext> pVRP{new ProblemContext{}};
  pVRP->load(scanner);
  OptFrameCoreVRP demo{pVRP};

  // build initial solution (test)
  VRP_RepADS rep = *demo.randomConstructive->generateSolution(0);
  std::cout << rep << std::endl;

  // evaluate
  ESolutionVRP se(rep, demo.evalVRP->evaluate(rep));
  std::cout << se.first << std::endl;
  se.second.print();

  // sref<NSSeq<ESolutionVRP>> nsseq_deltaIterator_delta_2opt = new
  // NSSeqVRP2Opt<ESolutionVRP, ProblemContext, DeltaMoveVRP2Opt,
  // DeltaNSIteratorVRP2Opt<DeltaMoveVRP2Opt>>(localGetRoutes, localGetRoutesX,
  // &pVRP);
  sref<NSSeq<ESolutionVRP>> nsseq_2opt =
      new NSSeqVRP2Opt<ESolutionVRP, ProblemContext>(localGetRoutes,
                                                     &pVRP.get());
  sref<NSSeq<ESolutionVRP>> nsseq_exchange =
      new NSSeqVRPExchange<ESolutionVRP, ProblemContext>(localGetRoutes,
                                                         &pVRP.get());
  sref<NSSeq<ESolutionVRP>> nsseq_or1 =
      new NSSeqVRPOrOpt1<ESolutionVRP, ProblemContext>(localGetRoutes,
                                                       &pVRP.get());
  sref<NSSeq<ESolutionVRP>> nsseq_or2 =
      new NSSeqVRPOrOpt2<ESolutionVRP, ProblemContext>(localGetRoutes,
                                                       &pVRP.get());
  //
  // uptr<Move<ESolutionVRP>> movetest =
  // nsseq_deltaIterator_delta_2opt->randomMove(esolTest);

  sref<LocalSearch<ESolutionVRP>> bi_2opt =
      new BestImprovement<ESolutionVRP>(demo.evalVRP, nsseq_2opt);
  bi_2opt->setVerboseR();
  SearchStatus st = bi_2opt->searchFrom(se, {10});
  std::cout << "status = " << (int)st << std::endl;
  std::cout << se.first << std::endl;
  se.second.print();

  vsref<LocalSearch<ESolutionVRP>> ls_vnd;
  ls_vnd.push_back(bi_2opt);
  ls_vnd.push_back(
      new BestImprovement<ESolutionVRP>(demo.evalVRP, nsseq_exchange));
  ls_vnd.push_back(new BestImprovement<ESolutionVRP>(demo.evalVRP, nsseq_or1));
  ls_vnd.push_back(new BestImprovement<ESolutionVRP>(demo.evalVRP, nsseq_or2));

  sref<LocalSearch<ESolutionVRP>> vnd =
      new VariableNeighborhoodDescent<ESolutionVRP>(demo.evalVRP, ls_vnd,
                                                    myRandGen.sptr());

  vnd->setVerboseR();
  SearchStatus st2 = vnd->searchFrom(se, {10});
  std::cout << "status = " << (int)st2 << std::endl;
  std::cout << se.first << std::endl;
  se.second.print();

  /*
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
           */

  return 0;
}