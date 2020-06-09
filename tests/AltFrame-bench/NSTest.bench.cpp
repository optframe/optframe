#include <iostream>

//#include <benchmark/benchmark.h>
#include <random>
#include <vector>

#include <AltFrame/NSSeqTest.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

using namespace std; // whatever
using namespace optframe; // Matrix
using namespace scannerpp; // Scanner++

// dummy class
template<class X>
class Evaluation
{
public:

   Evaluation(X x)
   {
   }

};

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
//using namespace TSP_fcore;

// set problem and return solution
ESolutionTSP
setTSP(unsigned N, long seed)
{
   pTSP.n = N;
   std::vector<int> vx, vy;
   srand(seed);
   for (unsigned i = 0; i < N; i++) {
      vx.push_back(rand() % 100);
      vy.push_back(rand() % 100);
   }
   pTSP.dist = Matrix<double>{ N, N };
   // calculate distance values, for every client pair (i,j)
   for (unsigned i = 0; i < N; i++)
      for (unsigned j = 0; j < N; j++)
         pTSP.dist(i, j) = pTSP.distance(vx.at(i), vy.at(i), vx.at(j), vy.at(j));

   // return solution
   std::vector<int> v(N);
   for (unsigned i = 0; i < N; i++)
      v[i] = i;
   return ESolutionTSP(v, Evaluation<double>{ 0 });
}

// =================
//      bench
// =================


static void
TSP_hardcoded_CPP_Swap_iteration(benchmark::State& state)
{
   unsigned N = state.range(0);    // get N from benchmark suite
   unsigned seed = state.range(1); // get seed from benchmark suite
   double ff = 0;
   for (auto _ : state) {
      state.PauseTiming();
      auto esol = setTSP(N, seed); // TODO: fixtures
      state.ResumeTiming();
      //
      double best = 99999999;
      std::pair<int, int> mij(-1, -1);
      // compute swap loop
      for (int i = 0; i < pTSP.n - 1; ++i)
         for (int j = i + 1; j < pTSP.n; ++j) {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            int aux = esol.first[i];
            esol.first[i] = esol.first[j];
            esol.first[j] = aux;
            //
            // compute cost
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            int aux2 = esol.first[i];
            esol.first[i] = esol.first[j];
            esol.first[j] = aux2;
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_hardcoded_CPP_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

//
static void
TSP_reveng_Middle_Ref_abstract(benchmark::State& state)
{
   unsigned N = state.range(0);    // get N from benchmark suite
   unsigned seed = state.range(1); // get seed from benchmark suite
   double ff = 0;
   for (auto _ : state) {
      state.PauseTiming();
      auto esol = setTSP(N, seed); // TODO: fixtures
      state.ResumeTiming();
      //
      double best = 99999999;
      std::pair<int, int> mij(-1, -1);
      //
      // The mere existence of this class causes overhead!!!
      // How is this possible?
      //
      //NSSeqFuncListStateAbstract<std::vector<int>, MoveByContextRef<X>>
      /*
      NSSeqContextByRef<std::vector<int>, std::pair<int,int>> nsseq
      {
         [&nsseq](std::vector<int>& v) -> void {
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            // swap
            int aux = v[i];
            v[i] = v[j];
            v[j] = aux;
         }
      };
      */
      //
      //NSSeqFuncListStateAbstract<std::vector<int>, MoveSpecificByContextRef<std::vector<int>>>
      NSSeqContextSpecificByRef nsseq;
      //
      std::pair<int,int>& mpair = nsseq.commonState; 
      

      // NOTE THAT 'nsseq' IS UNUSED HERE!!! JUST CONSIDERING IT'S "EXISTENCE" OVERHEAD!

      //std::pair<int,int> mpair;
      /*
      auto myfuncDo = [&mpair](std::vector<int>& v) -> void {
                        int& i = mpair.first;
                        int& j = mpair.second;
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     };
      */
      //auto myfuncUndo = myfuncDo;

      //void(*fX)(std::vector<int>&) { myfuncDo };
      //std::function<void(std::vector<int>&)> fX { myfuncDo };

      //MoveMiddle<std::vector<int>> middle( fX );
      // compute swap loop
      for (int i = 0; i < pTSP.n - 1; ++i)
         for (int j = i + 1; j < pTSP.n; ++j) {
            //ff += v; // benchmark::DoNotOptimize(...)
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            mpair.first = i;
            mpair.second = j;
            
            //MoveByContextRef<std::vector<int>> mv = nsseq.getStateMove();
            //MoveSpecificByContextRef<std::vector<int>>
            auto mv = nsseq.getStateMove();

            //myfuncDo(v);
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            //myfuncDo(v);
            mv.fApplyDo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_reveng_Middle_Ref_abstract)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ;

