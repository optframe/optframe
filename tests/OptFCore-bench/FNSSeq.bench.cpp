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

// using functions directly with IMS
static void
TSP_IMS_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
      NSSeqSwapBoring myNSSwap; // avoid disappearing
      //
      //using IMS = NSSeqSwapBoring::ims_type;
      //using IMS = myNSSwap.ims_type;
      //IMS ims = NSSeqSwapBoring::ft_iterator(esol); // TODO: use auto
      auto ims = myNSSwap.sf_iterator(esol);
      for (myNSSwap.sf_first(ims); !myNSSwap.sf_isdone(ims); myNSSwap.sf_next(ims)) {
         //  static auto ft_current = fCurrent;
         // avoiding to get pointer.. just "ims" with plain C++!
         int i = ims.first;
         int j = ims.second;

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
//
BENCHMARK(TSP_IMS_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// using functions directly with IMS (bench uptr and apply)
static void
TSP_IMS_uptr_noapply_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
      NSSeqSwapBoring myNSSwap; // avoid disappearing
      //
      //using IMS = NSSeqSwapBoring::ims_type;
      //using IMS = myNSSwap.ims_type;
      //IMS ims = NSSeqSwapBoring::ft_iterator(esol); // TODO: use auto
      auto ims = myNSSwap.sf_iterator(esol); // TODO: use auto
      for (myNSSwap.sf_first(ims); !myNSSwap.sf_isdone(ims); myNSSwap.sf_next(ims)) {
         // now we get uptr!
         MoveSwap& mswap = (MoveSwap&)*myNSSwap.sf_current(ims);
         int i = mswap.m.first;
         int j = mswap.m.second;

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
//
BENCHMARK(TSP_IMS_uptr_noapply_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// using functions directly with IMS (bench uptr and apply)
static void
TSP_IMS_uptr_apply_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
      NSSeqSwapBoring myNSSwap; // avoid disappearing
      //
      //using IMS = NSSeqSwapBoring::ims_type;
      //using IMS = myNSSwap.ims_type;
      //IMS ims = NSSeqSwapBoring::ft_iterator(esol); // TODO: use auto
      auto ims = myNSSwap.sf_iterator(esol); // TODO: use auto
      for (myNSSwap.sf_first(ims); !myNSSwap.sf_isdone(ims); myNSSwap.sf_next(ims)) {
         // now we get uptr!
         uptr<Move<ESolutionTSP>> mv = myNSSwap.sf_current(ims);
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
BENCHMARK(TSP_IMS_uptr_apply_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

static void
TSP_CPP_apply_stack_Swap_iteration(benchmark::State& state)
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
            // stack alloc moves
            MoveSwap mswap(make_pair(i, j));
            // apply
            mswap.apply(esol);
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
            mswap.apply(esol);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_CPP_apply_stack_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// will apply move function directly (no need for move stack)
static void
TSP_CPP_fapply_nostack_Swap_iteration(benchmark::State& state)
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
            // no stack alloc move, just pair
            std::pair<int, int> m(i, j);
            // apply
            MoveSwap::fs_apply(m, esol);
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
            MoveSwap::fs_apply(m, esol);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_CPP_fapply_nostack_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// TODO: move this to container part
template<class XES = int>
class MoveXF
{
protected:
   using Self = MoveXF<XES>;
   //
   std::function<void()> fPrint;
   std::function<op<Self>(XES&)> fApply;

public:
   // slice copying
   MoveXF(const Self& m) noexcept
     : fPrint(m.fPrint)
     , fApply(m.fApply)
   {
   }

   MoveXF(Self&& m) noexcept
     : fPrint(std::move(m.fPrint))
     , fApply(std::move(m.fApply))
   {
   }

   // "empty" constructor
   MoveXF(std::function<void()> _fPrint, std::function<op<Self>(XES&)> _fApply) noexcept
     : fPrint(_fPrint)
     , fApply(_fApply)
   {
   }

protected:
   // do not keep this if "pure virtual"
   MoveXF() = delete;

public:
   // why do we need this?
   virtual ~MoveXF()
   {
   }

   virtual op<Self> apply(XES& se) final
   {
      return fApply(se);
   }

   virtual void print() final
   {
      fPrint();
   }
};

class MyMoveX final : public MoveXF<std::vector<int>>
{
   using XES = std::vector<int>;

public:
   MyMoveX(int x, int y)
     : MoveXF<XES>{ super(x, y) }
   {
   }

   static MoveXF<XES> super(int x, int y)
   {
      return MoveXF<XES>{
         //[x, y]() { std::cout << "x=" << x << ",y=" << y << std::endl; },
         [x, y]() { stPrint(x, y); }, // print x, y
         [x, y](XES& v) -> op<MoveXF<XES>> {
            // swap
            int aux = v[x];
            v[x] = v[y];
            v[y] = aux;
            return op<MoveXF<XES>>{
               MoveXF<XES>{
                 [x, y]() { stPrint(y, x); }, // print y, x
                 [x, y](XES& v) -> op<MoveXF<XES>> {
                    // swap
                    int aux = v[y];
                    v[y] = v[x];
                    v[x] = aux;
                    return nullopt; // no return
                 } }
            }; // end return
         }     // end apply
      };       // end 'super' constructor
   }           // end 'super' method

   static void stPrint(int x, int y)
   {
      std::cout << "x=" << x << ",y=" << y << std::endl;
   }
};

// stack-based 'current()' iterator (with IMS structure) - prototype!
static MoveXF<std::vector<int>>
stackCurrent(std::pair<int, int>& ims)
{
   return MoveXF<std::vector<int>>{
      MyMoveX::super(ims.first, ims.second)
   };
}

// stack-based 'current()' iterator (with IMS structure) - prototype!
static op<MoveXF<std::vector<int>>>
stackOpCurrent(std::pair<int, int>& ims)
{
   return make_optional(
     MoveXF<std::vector<int>>{
       MyMoveX::super(ims.first, ims.second) });
}

// will apply move function directly (no need for move stack)
static void
TSP_CPP_stackOpCurrent_Swap_iteration(benchmark::State& state)
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
            // no stack alloc move, just pair
            std::pair<int, int> ims(i, j);
            auto opMove = stackOpCurrent(ims);
            // apply
            auto opMoveRev = opMove->apply(esol.first);
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
            opMoveRev->apply(esol.first);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_CPP_stackOpCurrent_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// will apply move function directly (no need for move stack)
static void
TSP_CPP_stackCurrent_Swap_iteration(benchmark::State& state)
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
            // no stack alloc move, just pair
            std::pair<int, int> ims(i, j);
            auto move = stackCurrent(ims);
            // apply
            auto opMoveRev = move.apply(esol.first);
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
            opMoveRev->apply(esol.first);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_CPP_stackCurrent_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// Sol = vector<int>
template<
  class XES,
  class MStruct, // pair?
  void (*fPrint)(const MStruct&),
  op<MoveXF<XES>> (*fApply)(const MStruct&, XES&)>
class StackMoveX final : public MoveXF<XES>
{
   //using XES = std::vector<int>;

public:
   StackMoveX(const MStruct& m)
     : MoveXF<XES>{ super(m) }
   {
   }

   static MoveXF<XES> super(MStruct m)
   {
      return MoveXF<XES>{
         [m]() { fPrint(m); },
         [m](XES& vec) -> op<MoveXF<XES>> {
            return fApply(m, vec);
         } // end apply
      };   // end 'super' constructor
   }       // end 'super' method
};

// implementation using templated lambdas
using MoveSwapStackX = StackMoveX<
  std::vector<int>,
  std::pair<int, int>,
  // fPrint
  [](const std::pair<int, int>& m) {
     int x = m.first;
     int y = m.second;
     std::cout << "x=" << x << ",y=" << y << std::endl;
  },
  // fApply
  [](const std::pair<int, int>& m, std::vector<int>& v) -> op<MoveXF<std::vector<int>>> {
     int x = m.first;
     int y = m.second;
     // swap
     int aux = v[x];
     v[x] = v[y];
     v[y] = aux;
     return op<MoveXF<std::vector<int>>>{
        MoveXF<std::vector<int>>{
          [x, y]() { /*stPrint(y, x);*/ }, // print y, x
          [x, y](std::vector<int>& v) -> op<MoveXF<std::vector<int>>> {
             // swap
             int aux = v[y];
             v[y] = v[x];
             v[x] = aux;
             return nullopt; // no return
          } }
     }; // end return
  }     // end apply
  >;

// stack-based 'current()' iterator (with IMS structure) - prototype!
static MoveXF<std::vector<int>>
stackXCurrent(std::pair<int, int>& ims)
{
   return MoveXF<std::vector<int>>{
      MoveSwapStackX::super(ims)
   };
}

static void
TSP_CPP_stackXCurrent_Swap_iteration(benchmark::State& state)
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
            // no stack alloc move, just pair
            std::pair<int, int> ims(i, j);
            auto move = stackXCurrent(ims);
            // apply
            auto opMoveRev = move.apply(esol.first);
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
            opMoveRev->apply(esol.first);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_CPP_stackXCurrent_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

static void
TSP_SimpleMove_CPP(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      std::vector<int>& v = esol.first;
      //
      int aux = v[i];
      v[i] = v[j];
      v[j] = aux;
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      // undo
      int aux2 = v[i];
      v[i] = v[j];
      v[j] = aux2;
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_CPP)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)

  ;

static void
TSP_SimpleMove_CPP_std_swap(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      std::vector<int>& v = esol.first;
      //
      std::swap(v[i], v[j]);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      // undo
      std::swap(v[i], v[j]);
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_CPP_std_swap)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)

  ;

static void
TSP_SimpleMove_MoveXF(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      std::pair<int, int> ims(i, j);
      MoveXF<std::vector<int>> move = stackXCurrent(ims);
      op<MoveXF<std::vector<int>>> undo = move.apply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      undo->apply(esol.first);
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_MoveXF)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

static void
TSP_SimpleMove_new_uptr(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      std::pair<int, int> ims(i, j);
      NSSeqSwapBoring myNSSwap; // avoid disappearing
      uptr<Move<ESolutionTSP>> mv = myNSSwap.sf_current(ims);
      //MoveSwap& mswap = (MoveSwap&)*mv;
      // apply
      uptr<Move<ESolutionTSP>> m_undo = mv->apply(esol);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      m_undo->apply(esol);
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_new_uptr)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;