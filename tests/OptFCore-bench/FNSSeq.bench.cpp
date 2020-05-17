#include <iostream>

//#include <benchmark/benchmark.h>
#include <random>
#include <vector>

#include <FCore-Examples/src/TSP-fcore.hpp>

using namespace TSP_fcore;

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

static void
TSP_CPP_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_CPP_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

template<class TestNSSeq>
static void
TSP_TestNSSeq_Swap_iteration(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   pTSP.n = N;
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
      TestNSSeq swapFancy;
      auto it1 = swapFancy.getIterator(esol);
      for (it1->first(); !it1->isDone(); it1->next()) {
         //it1->current()->print();
         uptr<Move<ESolutionTSP>> mv = it1->current();
         MoveSwap& mswap = (MoveSwap&)*mv;
         int i = mswap.m.first;
         int j = mswap.m.second;
         // apply
         uptr<Move<ESolutionTSP>> m_undo = mv->apply(esol);
         //
         // compute cost
         double fcost;
         benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]); // fake
         if (fcost < best) {
            best = fcost;
            mij = make_pair(i, j);
         }
         //
         // undo
         m_undo->apply(esol);
      }

      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
//
BENCHMARK_TEMPLATE(TSP_TestNSSeq_Swap_iteration, NSSeqSwapFancy)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;
BENCHMARK_TEMPLATE(TSP_TestNSSeq_Swap_iteration, NSSeqSwapBoring)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;
