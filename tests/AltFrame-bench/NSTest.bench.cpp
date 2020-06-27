#include <iostream>

//#include <benchmark/benchmark.h>
#include <random>
#include <vector>

#include "../AltFrame/NSSeqTest.hpp"
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
TSP_AltFrame_1_CPP_baseline(benchmark::State& state)
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
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
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
BENCHMARK(TSP_AltFrame_1_CPP_baseline)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;

//
static void
TSP_AltFrame_NSSeqTestPtr_MoveFPtr(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            MoveFPtr mv { nsseq.current().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveFPtr)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;

//
static void
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            const MoveFPtr<std::vector<int>>& mv { nsseq.staticMoveFPtr.value() }; //{ nsseq.ref_fCurrent().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;

//
static void
TSP_AltFrame_NSSeqTestPtr_MoveFRef(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            MoveFRef mv { nsseq.fCurrentRef().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveFRef)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;

//
static void
TSP_AltFrame_NSSeqTestPtr_MoveFRefTL(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            MoveFTL mv { nsseq.fCurrentRefTL().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveFRefTL)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;



//
static void
TSP_AltFrame_NSSeqTestPtr_MoveFCopy(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            MoveFCopy mv { nsseq.fCurrentMoveFCopy().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveFCopy)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;


//
static void
TSP_AltFrame_NSSeqTestPtr_MoveInternal(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.first(v); !nsseq.isDone(); nsseq.next())
         {
            //auto mv = *nsseq.current();
            NSSeqTestStateless::MoveInternal mv { nsseq.fCurrentMoveInternal().value() };
            //mv.fApplyDo(v);
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_MoveInternal)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;


//
// heap allocation
static void
TSP_AltFrame_NSSeqTestPtr_iter_heap(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //uptr< NSIterator<std::vector<int>, NSSeqTestPtr::MyMove >>
      //
      std::vector<int>& v = esol.first;
      //
      // uptr<NSIterator<std::vector<int>, MyMove>>
      auto iter = nsseq.getIterator(v);
      //     
      // SHORT ITERATOR...
         for(iter->first(v); !iter->isDone(); iter->next())
         {
            //auto mv = *nsseq.current();
            MoveFPtr mv = *iter->current();
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_iter_heap)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;


// get Iterator class on stack
static void
TSP_AltFrame_NSSeqTestPtr_iter_stack(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //uptr< NSIterator<std::vector<int>, NSSeqTestPtr::MyMove >>
      //
      std::vector<int>& v = esol.first;
      //
      auto iter = nsseq.getIteratorCopy(v);
      //     
      // SHORT ITERATOR...
         for(iter.fFirst(v); !iter.fIsDone(); iter.fNext())
         {
            //auto mv = *nsseq.current();
            MoveFPtr mv { iter.fCurrent().value() };
            mv.apply(v);
            // compute cost
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_iter_stack)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;

#ifdef CORO // enable coroutines
static void
TSP_AltFrame_NSSeqTestPtr_raw_coro(benchmark::State& state)
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
      NSSeqTestStateless nsseq{pTSP.n};
      //
      std::vector<int>& v = esol.first;
      // SHORT ITERATOR...
         for(nsseq.coroFirst(v); !nsseq.coroIsDone(); nsseq.coroNext())
         {
            MoveFPtr mv { nsseq.coroCurrent().value() };
            mv.apply(v);
            //
            // compute cost
            // THIS IS ONLY POSSIBLE HERE DURING SPECIFIC TESTS...
            // ON PRACTICE, NSFind should handle state copies, not possible on MoveRef type.
            int& i = nsseq.commonState.first;
            int& j = nsseq.commonState.second;
            double fcost;
            benchmark::DoNotOptimize(fcost = esol.first[0] + esol.first[3]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.undo(v); // TODO: apply undo
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
      assert(ff == 1);
   }
}
BENCHMARK(TSP_AltFrame_NSSeqTestPtr_raw_coro)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ->Args({ 1000, 0 }) // N = 10 - seed 0
  ;
#endif