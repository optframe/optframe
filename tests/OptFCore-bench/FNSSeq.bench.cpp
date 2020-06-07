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

template<class TestNSSeq>
static void
TSP_generic_TestNSSeq_Swap_iteration(benchmark::State& state)
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
BENCHMARK_TEMPLATE(TSP_generic_TestNSSeq_Swap_iteration, NSSeqSwapFancy)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;
BENCHMARK_TEMPLATE(TSP_generic_TestNSSeq_Swap_iteration, NSSeqSwapBoring)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// using functions directly with IMS
static void
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// using functions directly with IMS (bench uptr and apply)
static void
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// using functions directly with IMS (bench uptr and apply)
static void
TSP_generic_IMS_uptr_apply_NSSeqBoring_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_generic_IMS_uptr_apply_NSSeqBoring_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

static void
TSP_hardcoded_CPP_apply_stack_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_hardcoded_CPP_apply_stack_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// will apply move function directly (no need for move stack)
static void
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_hardcoded_CPP_fapply_nostack_Swap_iteration)
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
   constexpr MoveXF(const Self& m) noexcept
     : fPrint(m.fPrint)
     , fApply(m.fApply)
   {
   }

   constexpr MoveXF(Self&& m) noexcept
     : fPrint(std::move(m.fPrint))
     , fApply(std::move(m.fApply))
   {
   }

   // "empty" constructor
   constexpr MoveXF(std::function<void()> _fPrint, std::function<op<Self>(XES&)> _fApply) noexcept
     : fPrint(_fPrint)
     , fApply(_fApply)
   {
   }

protected:
   // do not keep this if "pure virtual"
   MoveXF() = delete;

public:
   // why do we need this?
   constexpr ~MoveXF()
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

   constexpr ~MyMoveX()
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
TSP_generic_CPP_stackOpCurrent_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_generic_CPP_stackOpCurrent_Swap_iteration)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// will apply move function directly (no need for move stack)
static void
TSP_generic_CPP_stackCurrent_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_generic_CPP_stackCurrent_Swap_iteration)
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
   constexpr StackMoveX(const MStruct& m)
     : MoveXF<XES>{ super(m) }
   {
   }

   constexpr static MoveXF<XES> super(MStruct m)
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
TSP_generic_CPP_stackXCurrent_Swap_iteration(benchmark::State& state)
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
BENCHMARK(TSP_generic_CPP_stackXCurrent_Swap_iteration)
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
TSP_SimpleMove_StackCurrent(benchmark::State& state)
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
      MoveXF<std::vector<int>> move = stackCurrent(ims);
      op<MoveXF<std::vector<int>>> undo = move.apply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      undo->apply(esol.first);
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_StackCurrent)
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

static void
TSP_SimpleMove_StackXCurrent(benchmark::State& state)
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
BENCHMARK(TSP_SimpleMove_StackXCurrent)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

static void
TSP_SimpleMove_MoveXF_nocopy(benchmark::State& state)
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
      MoveXF<std::vector<int>> move{
         MoveSwapStackX::super(ims)
      };
      op<MoveXF<std::vector<int>>> undo = move.apply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      undo->apply(esol.first);
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_MoveXF_nocopy)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

// ===================================== NoUndo tests =====================================
// ===================================== NoUndo tests =====================================
// ===================================== NoUndo tests =====================================
// ===================================== NoUndo tests =====================================

static void
TSP_SimpleMove_NoUndo_CPP_std_swap(benchmark::State& state)
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
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_CPP_std_swap)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

static void
TSP_SimpleMove_NoUndo_new_uptr(benchmark::State& state)
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
      mv->apply(esol);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_new_uptr)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

static void
TSP_SimpleMove_NoUndo_MyMoveX_nocopy(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      //std::pair<int, int> ims(i, j);
      MoveXF<std::vector<int>> move{
         std::move(MyMoveX::super(i, j))
      };
      move.apply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_MyMoveX_nocopy)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

template<class XES>
MoveXF<XES>
func_MyMoveX_super(int x, int y)
{
   return MoveXF<XES>{
      //[x, y]() { std::cout << "x=" << x << ",y=" << y << std::endl; },
      [x, y]() {}, // print x, y
      [x, y](XES& v) -> op<MoveXF<XES>> {
         // swap
         int aux = v[x];
         v[x] = v[y];
         v[y] = aux;
         return op<MoveXF<XES>>{
            MoveXF<XES>{
              [x, y]() {}, // print y, x
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
} // end 'super' method

static void
TSP_SimpleMove_NoUndo_MyMoveX_func(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      //std::pair<int, int> ims(i, j);
      MoveXF<std::vector<int>> move{
         std::move(func_MyMoveX_super<std::vector<int>>(i, j))
      };
      move.apply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_MyMoveX_func)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

// --------------------------------------

template<class XES>
std::function<void(XES&)>
getfunc_MyMoveX_apply_noundo(int x, int y)
{
   return
     [x, y](XES& v) -> void {
        // swap
        int aux = v[x];
        v[x] = v[y];
        v[y] = aux;
     };
}

// ==================================
// THIS IS VERY FAST!! C++ level fast
// ==================================
static void
TSP_SimpleMove_NoUndo_MyMoveX_func_noundo(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      //std::pair<int, int> ims(i, j);
      auto fApply = getfunc_MyMoveX_apply_noundo<std::vector<int>>(i, j);
      fApply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_MyMoveX_func_noundo)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

// -----------------------------------

template<class XES>
class MoveFuncList final
{
public:
   std::function<void(XES&)> fApply;

   MoveFuncList(std::function<void(XES&)> _fApply)
     : fApply(_fApply)
   {
   }
};


class NSSeqFuncList
{
public:
   MoveFuncList<std::vector<int>> getApplyTSPSwapNoUndo(int x, int y)
   {
      return MoveFuncList<std::vector<int>>{
         [x, y](std::vector<int>& v) -> void {
            // swap
            int aux = v[x];
            v[x] = v[y];
            v[y] = aux;
         }
      };
   }

};

static void
TSP_SimpleMove_NoUndo_MoveFuncList_apply(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      //std::pair<int, int> ims(i, j);
      NSSeqFuncList nsswap;
      MoveFuncList<std::vector<int>> moveflist = nsswap.getApplyTSPSwapNoUndo(i, j);
      auto fApply = moveflist.fApply;
      fApply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_MoveFuncList_apply)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;


// --------------------------

template<class XES>
class MoveFuncListConstexpr final
{
public:
   std::function<void(XES&)> fApply;

   constexpr MoveFuncListConstexpr(const std::function<void(XES&)>& _fApply)
     : fApply{_fApply}
   {
   }
};

class NSSeqFuncListConstexpr
{
public:
   MoveFuncListConstexpr<std::vector<int>> getApplyTSPSwapNoUndo(int x, int y)
   {
      return MoveFuncListConstexpr<std::vector<int>>{
         [x, y](std::vector<int>& v) -> void {
            // swap
            int aux = v[x];
            v[x] = v[y];
            v[y] = aux;
         }
      };
   }
};

static void
TSP_SimpleMove_NoUndo_MoveFuncListConstexpr_apply(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      //std::pair<int, int> ims(i, j);
      NSSeqFuncList nsswap;
      MoveFuncList<std::vector<int>> moveflist = nsswap.getApplyTSPSwapNoUndo(i, j);
      auto fApply = moveflist.fApply;
      fApply(esol.first);
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
   }
}
BENCHMARK(TSP_SimpleMove_NoUndo_MoveFuncListConstexpr_apply)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ;

// ---------------------------
// back to Undo situation


template<class XES>
class MoveUndoFuncList final
{
public:
   //using undo_func = std::optional<std::function<void(XES&)>; // no 'undo' of 'undo'
   //using complete_func = std::function<undo_func(XES&)>;
   //complete_func fApplyUndo;

   std::function<void(XES&)> fApplyDo;
   std::function<void(XES&)> fApplyUndo;

   MoveUndoFuncList(std::function<void(XES&)> _fApplyDoUndo)
     : fApplyDo{_fApplyDoUndo}, fApplyUndo{_fApplyDoUndo}
   {
   }

   MoveUndoFuncList(std::function<void(XES&)> _fApplyDo, std::function<void(XES&)> _fApplyUndo)
     : fApplyDo{_fApplyDo}, fApplyUndo{_fApplyUndo}
   {
   }
};

template<class X>
class NSSeqFuncListVirtual
{
public:
   virtual MoveUndoFuncList<X> getMove() = 0;

};

class NSSeqFuncListUndo : public NSSeqFuncListVirtual<std::vector<int>>
{
public:

   int x{0};
   int y{0};

   constexpr NSSeqFuncListUndo(int _x, int _y) :
      x{_x}, y{_y}
   {
   }
   
   virtual MoveUndoFuncList<std::vector<int>> getMove() override
   {
      return getApplyTSPSwap(2, 5); 
   }

private:
   // This allows Do and Undo
   MoveUndoFuncList<std::vector<int>> getApplyTSPSwap(int x, int y)
   {
      return MoveUndoFuncList<std::vector<int>>{
         [x, y](std::vector<int>& v) -> void {
            // swap
            int aux = v[x];
            v[x] = v[y];
            v[y] = aux;
         }
      };
   }
};


static void
TSP_SimpleMove_MoveUndoFuncList(benchmark::State& state)
{
   unsigned N = state.range(0); // get N from benchmark suite
   //unsigned seed = state.range(1); // get seed from benchmark suite
   unsigned i = state.range(2);
   unsigned j = state.range(3);
   //
   NSSeqFuncListUndo ns{(int)i,(int)j};
   for (auto _ : state) {
      state.PauseTiming();
      ESolutionTSP esol{ std::vector<int>(N, 0), Evaluation<double>{} }; // empty solution
      state.ResumeTiming();
      std::vector<int>& v = esol.first;
      //
      auto mv = ns.getMove();
      mv.fApplyDo(v);
      //
      double fcost; // fake
      benchmark::DoNotOptimize(fcost = esol.first[i] + esol.first[j]);
      // undo
      mv.fApplyUndo(v);
      //
      benchmark::DoNotOptimize(fcost += esol.first[i] + esol.first[j]);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_SimpleMove_MoveUndoFuncList)
  ->Args({ 10, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 20, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)
  ->Args({ 30, 0, 2, 5 }) // N = 10, seed=0, i=2, j=5 (swap)

  ;

// ---------------------------------
// trying to perform full loop again
//          FINAL tests
// ---------------------------------

static void
TSP_final_baseline_CPP(benchmark::State& state)
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
BENCHMARK(TSP_final_baseline_CPP)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ;



static void
TSP_final_MoveUndoFuncList(benchmark::State& state)
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
            NSSeqFuncListUndo ns{(int)i,(int)j};
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            auto mv = ns.getMove();
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

/*
--------------------------------------------------------------------------
Benchmark                                Time             CPU   Iterations
--------------------------------------------------------------------------
TSP_final_baseline_CPP/10/0            566 ns          564 ns      1256353
TSP_final_baseline_CPP/20/0            940 ns          938 ns       755174
TSP_final_baseline_CPP/30/0           1596 ns         1594 ns       432643
TSP_final_MoveUndoFuncList/10/0       1111 ns         1108 ns       646330
TSP_final_MoveUndoFuncList/20/0       2922 ns         2920 ns       239729
TSP_final_MoveUndoFuncList/30/0       6164 ns         6163 ns       117237
*/

// the baseline CPP is still much better than 'MoveUndoFuncList'

// =======================================================================
//

static void
TSP_final_MoveUndoFuncList_Raw(benchmark::State& state)
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
            NSSeqFuncListUndo ns{(int)i,(int)j};
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = [i, j](std::vector<int>& v) -> void {
               int aux = v[i];
               v[i] = v[j];
               v[j] = aux;
            };
            mv(v);
            //
            // compute cost
            double fcost;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv(v);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

  /*
  ------------------------------------------------------------------------------
Benchmark                                    Time             CPU   Iterations
------------------------------------------------------------------------------
TSP_final_baseline_CPP/10/0                561 ns          560 ns      1260464
TSP_final_baseline_CPP/20/0                945 ns          941 ns       747021
TSP_final_baseline_CPP/30/0               1604 ns         1599 ns       435569
TSP_final_MoveUndoFuncList/10/0           1088 ns         1088 ns       619682
TSP_final_MoveUndoFuncList/20/0           3009 ns         3007 ns       227840
TSP_final_MoveUndoFuncList/30/0           6325 ns         6319 ns       104074
TSP_final_MoveUndoFuncList_Raw/10/0        561 ns          560 ns      1260307
TSP_final_MoveUndoFuncList_Raw/20/0        935 ns          932 ns       752726
TSP_final_MoveUndoFuncList_Raw/30/0       1602 ns         1597 ns       418978
*/

template<class X>
class NSSeqFuncListUndoNoInherit final
{
public:
   std::function<MoveUndoFuncList<X>()> getMove;

   NSSeqFuncListUndoNoInherit(std::function<MoveUndoFuncList<X>()> _getMove) :
      getMove{_getMove}
   {
   }
};


static void
TSP_final_MoveUndoFuncList_Raw2(benchmark::State& state)
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
            //
            std::function<MoveUndoFuncList<std::vector<int>>()> myfunc = 
               [i,j] () -> MoveUndoFuncList<std::vector<int>>
               {
                  return MoveUndoFuncList<std::vector<int>>(
                     [i, j](std::vector<int>& v) -> void {
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     }
                  );
               };
            //
            NSSeqFuncListUndoNoInherit<std::vector<int>> nsseq (
               //
               myfunc
            );
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = nsseq.getMove(); // apply function and get move
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw2)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// every generalization makes things worse...

  /*
TSP_final_MoveUndoFuncList_Raw/10/0         559 ns          558 ns      1264497
TSP_final_MoveUndoFuncList_Raw/20/0         939 ns          935 ns       756946
TSP_final_MoveUndoFuncList_Raw/30/0        1615 ns         1610 ns       441730
TSP_final_MoveUndoFuncList_Raw2/10/0       1580 ns         1579 ns       442634
TSP_final_MoveUndoFuncList_Raw2/20/0       5077 ns         5077 ns       137195
TSP_final_MoveUndoFuncList_Raw2/30/0      11342 ns        11340 ns        64033
*/

// ========================================


template<class X, class State>
class NSSeqFuncListState final
{
public:

   std::function<MoveUndoFuncList<X>(State& state)> getStateMove;

   std::function<void(State& state)> getNextState;

   std::function<bool(State& state)> getDone;

   NSSeqFuncListState(
      std::function<MoveUndoFuncList<X>(State&)> _getStateMove,
      std::function<void(State&)> _getNextState,
      std::function<bool(State&)> _getDone
   ) :
      getStateMove{_getStateMove}, getNextState{_getNextState}, getDone{_getDone}
   {
   }
};


static void
TSP_final_MoveUndoFuncList_Raw_State(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      //
      NSSeqFuncListState<std::vector<int>, std::pair<int, int>> nsseq(
         [] (std::pair<int, int>& st) -> MoveUndoFuncList<std::vector<int>>
               {
                  int i = st.first;
                  int j = st.second;
                  return MoveUndoFuncList<std::vector<int>>(
                     [i, j](std::vector<int>& v) -> void {
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     }
                  );
               },
               [] (std::pair<int, int>& st) -> void
               {
                  st.second++;
                  if(st.second >= pTSP.n)
                  {
                     st.first++;
                     st.second = st.first+1;
                  }
               },
               [] (std::pair<int, int>& st) -> bool
               {
                  int i = st.first;
                  return i >= (pTSP.n - 1);
               }
      );
      // compute swap loop
      while(!nsseq.getDone(st))
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = nsseq.getStateMove(st); // apply function and get move
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            nsseq.getNextState(st);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// everything makes it slower...

/*
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
TSP_final_baseline_CPP/10/0                      563 ns          562 ns      1254824
TSP_final_baseline_CPP/20/0                      937 ns          937 ns       733637
TSP_final_baseline_CPP/30/0                     1609 ns         1606 ns       435310
TSP_final_MoveUndoFuncList/10/0                 1071 ns         1072 ns       652614
TSP_final_MoveUndoFuncList/20/0                 2974 ns         2973 ns       236855
TSP_final_MoveUndoFuncList/30/0                 6052 ns         6053 ns       115802
TSP_final_MoveUndoFuncList_Raw/10/0              560 ns          560 ns      1252980
TSP_final_MoveUndoFuncList_Raw/20/0              938 ns          938 ns       746896
TSP_final_MoveUndoFuncList_Raw/30/0             1613 ns         1609 ns       436915
TSP_final_MoveUndoFuncList_Raw2/10/0            1582 ns         1582 ns       428814
TSP_final_MoveUndoFuncList_Raw2/20/0            5047 ns         5049 ns       138408
TSP_final_MoveUndoFuncList_Raw2/30/0           10877 ns        10884 ns        64090
TSP_final_MoveUndoFuncList_Raw_State/10/0       1477 ns         1484 ns       475452
TSP_final_MoveUndoFuncList_Raw_State/20/0       4295 ns         4309 ns       162140
TSP_final_MoveUndoFuncList_Raw_State/30/0       9107 ns         9118 ns        77857
*/

auto myGetMoveGlobal = [] (std::pair<int, int>& st) -> MoveUndoFuncList<std::vector<int>>
               {
                  int i = st.first;
                  int j = st.second;
                  return MoveUndoFuncList<std::vector<int>>(
                     [i, j](std::vector<int>& v) -> void {
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     }
                  );
               };

auto myNextStateGlobal = [] (std::pair<int, int>& st) -> void
               {
                  st.second++;
                  if(st.second >= pTSP.n)
                  {
                     st.first++;
                     st.second = st.first+1;
                  }
               };

auto myIsDoneGlobal = [] (std::pair<int, int>& st) -> bool
               {
                  int i = st.first;
                  return i >= (pTSP.n - 1);
               };

static void
TSP_final_MoveUndoFuncList_Raw_State_Global(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      //
      /*
      NSSeqFuncListState<std::vector<int>, std::pair<int, int>> nsseq(
               myGetMoveGlobal,
               myNextStateGlobal,
               myIsDoneGlobal
      );
      */
      // compute swap loop
      //while(!nsseq.getDone(st))
      while(!myIsDoneGlobal(st))
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            //auto mv = nsseq.getStateMove(st); // apply function and get move
            auto mv = myGetMoveGlobal(st);
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            //nsseq.getNextState(st);
            myNextStateGlobal(st);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State_Global)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

  /*
TSP_final_MoveUndoFuncList_Raw_State/10/0              1416 ns         1418 ns       493873
TSP_final_MoveUndoFuncList_Raw_State/20/0              4225 ns         4228 ns       164005
TSP_final_MoveUndoFuncList_Raw_State/30/0              9038 ns         9040 ns        78103
TSP_final_MoveUndoFuncList_Raw_State_Global/10/0       1135 ns         1137 ns       616657
TSP_final_MoveUndoFuncList_Raw_State_Global/20/0       3150 ns         3152 ns       224886
TSP_final_MoveUndoFuncList_Raw_State_Global/30/0       6384 ns         6386 ns       109318
*/


MoveUndoFuncList<std::vector<int>> myGetMoveGlobal2 (std::pair<int, int>& st)
               {
                  int i = st.first;
                  int j = st.second;
                  return MoveUndoFuncList<std::vector<int>>(
                     [i, j](std::vector<int>& v) -> void {
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     }
                  );
               };

void myNextStateGlobal2(std::pair<int, int>& st)
               {
                  st.second++;
                  if(st.second >= pTSP.n)
                  {
                     st.first++;
                     st.second = st.first+1;
                  }
               };

bool myIsDoneGlobal2 (std::pair<int, int>& st)
               {
                  int i = st.first;
                  return i >= (pTSP.n - 1);
               };

static void
TSP_final_MoveUndoFuncList_Raw_State_Global2(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      //
      /*
      NSSeqFuncListState<std::vector<int>, std::pair<int, int>> nsseq(
               myGetMoveGlobal,
               myNextStateGlobal,
               myIsDoneGlobal
      );
      */
      // compute swap loop
      //while(!nsseq.getDone(st))
      while(!myIsDoneGlobal2(st))
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            //auto mv = nsseq.getStateMove(st); // apply function and get move
            auto mv = myGetMoveGlobal2(st);
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            //nsseq.getNextState(st);
            myNextStateGlobal2(st);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State_Global2)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// --------------

template<
class X, 
class State, 
MoveUndoFuncList<X> (*XgetStateMove) (State& state),
void (*XgetNextState) (State& state),
bool (*XgetDone) (State& state)
>
class NSSeqFuncListState2 final
{
public:
   MoveUndoFuncList<X> (*getStateMove) (State& state) { XgetStateMove };
   void (*getNextState) (State& state) { XgetNextState };
   bool (*getDone) (State& state) { XgetDone };
};


static void
TSP_final_MoveUndoFuncList_Raw_State2(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      //
      NSSeqFuncListState2<
         std::vector<int>, 
         std::pair<int, int>,
         myGetMoveGlobal2,
         myNextStateGlobal2,
         myIsDoneGlobal2
      > nsseq;
      // compute swap loop
      while(!nsseq.getDone(st))
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = nsseq.getStateMove(st); // apply function and get move
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            nsseq.getNextState(st);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State2)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// -----------

// embedding functions by template
using NSSeqFuncListState3 = NSSeqFuncListState2<
         std::vector<int>, 
         std::pair<int, int>,
         myGetMoveGlobal,
         myNextStateGlobal,
         myIsDoneGlobal
      >;

static void
TSP_final_MoveUndoFuncList_Raw_State3(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      // embed functions
      NSSeqFuncListState3 nsseq;
      // compute swap loop
      while(!nsseq.getDone(st))
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = nsseq.getStateMove(st); // apply function and get move
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            nsseq.getNextState(st);
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State3)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ;

// ------------


// embedding functions by template
using NSSeqFuncListState3 = NSSeqFuncListState2<
         std::vector<int>, 
         std::pair<int, int>,
         myGetMoveGlobal,
         myNextStateGlobal,
         myIsDoneGlobal
      >;

template< class X >
class NSSeqFuncListState4 final
{
public:
   std::function<MoveUndoFuncList<X>()> getStateMove;
   std::function<void()> getNextState;
   std::function<bool()> getDone;
   //State state;

   template<
      class State
      //,
      //MoveUndoFuncList<X> (*XgetStateMove) (State& state),
      //void (*XgetNextState) (State& state),
      //bool (*XgetDone) (State& state)
   >
   NSSeqFuncListState4(
      State& _state, 
      //std::function<MoveUndoFuncList<X>(State&)> funcGetStateMove,
      //std::function<void(State&)> funcGetNextState,
      //std::function<bool(State&)> funcGetDone
      MoveUndoFuncList<X>(*funcGetStateMove)(State&),
      void(*funcGetNextState)(State&),
      bool(*funcGetDone)(State&)      
      //auto funcGetStateMove,
      //auto funcGetNextState,
      //auto funcGetDone
   )
   : 
   //state{_state},
   getStateMove{ [&_state, funcGetStateMove]()->MoveUndoFuncList<X>{return funcGetStateMove(_state);} },
   getNextState{ [&_state, funcGetNextState]()->void{return funcGetNextState(_state);} },
   getDone{ [&_state, funcGetDone]()->bool{return funcGetDone(_state);} }
   {
   }

   //MoveUndoFuncList<X> (*getStateMove) (State& state) { funcGetStateMove };
   //void (*getNextState) (State& state) { funcGetNextState };
   //bool (*getDone) (State& state) { funcGetDone };

};

static void
TSP_final_MoveUndoFuncList_Raw_State4(benchmark::State& state)
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
      std::pair<int, int> mij(-1, -1); // best value
      //
      // state
      std::pair<int, int> st(0, 1);
      // embed functions
      NSSeqFuncListState4 nsseq(
         st,          
         myGetMoveGlobal2,
         myNextStateGlobal2,
         myIsDoneGlobal2
      );
      // compute swap loop
      while(!nsseq.getDone())
      {
            //ff += v; // benchmark::DoNotOptimize(...)
            //
            // swap
            std::vector<int>& v = esol.first;
            //
            // HARDCODING FUNCTION HERE
            auto mv = nsseq.getStateMove(); // apply function and get move
            mv.fApplyDo(v);
            //
            // compute cost
            double fcost;
            int i = st.first;
            int j = st.second;
            benchmark::DoNotOptimize(fcost = v[i] + v[j]); // fake
            if (fcost < best) {
               best = fcost;
               mij = make_pair(i, j);
            }
            //
            // undo swap
            mv.fApplyUndo(v);
            //
            nsseq.getNextState();
         }
      benchmark::DoNotOptimize(ff = best);
      benchmark::ClobberMemory();
   }
}
BENCHMARK(TSP_final_MoveUndoFuncList_Raw_State4)
  ->Args({ 10, 0 }) // N = 10 - seed 0
  ->Args({ 20, 0 }) // N = 10 - seed 0
  ->Args({ 30, 0 }) // N = 10 - seed 0
  ->Args({ 100, 0 }) // N = 10 - seed 0
  ->Args({ 200, 0 }) // N = 10 - seed 0
  ;

// more generalism harms performance...
  /*
TSP_final_MoveUndoFuncList_Raw_State3/10/0              1109 ns         1107 ns       634319
TSP_final_MoveUndoFuncList_Raw_State3/20/0              3085 ns         3081 ns       226391
TSP_final_MoveUndoFuncList_Raw_State3/30/0              6561 ns         6557 ns       107399
TSP_final_MoveUndoFuncList_Raw_State4/10/0              1540 ns         1540 ns       442200
TSP_final_MoveUndoFuncList_Raw_State4/20/0              4881 ns         4882 ns       143472
TSP_final_MoveUndoFuncList_Raw_State4/30/0             10627 ns        10624 ns        63506
*/

// ================
/*
- Inheritance harms performance
- Class equality using advanced Type Erasure techniques and efficient Closures

Method declarations on global scope (static)
Efficient encapsulation of closure parameters... how???


*/

