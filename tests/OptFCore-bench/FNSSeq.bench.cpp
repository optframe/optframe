#include<iostream>

//#include <benchmark/benchmark.h>
#include <vector>
#include <random>

/*
#include <kahan.hpp> // from 'src'
#include <neumaier.hpp> // from 'src'

using namespace kahan;

std::set<int> genRandomSet(long seed, long count)
{
   std::default_random_engine engine;
  engine.seed(seed);
  std::uniform_int_distribution<int> runif(-1000,+1000);

   std::set<int> data;
   for(long c=0; c<count; ++c)
      data.insert(runif(engine));

   return data;
}


static void double_plus_assign_rand(benchmark::State &state)
{
   

  std::default_random_engine engine;
  engine.seed(0);
  std::uniform_int_distribution<int> runif(-1000,+1000);


   //while(state.KeepRunning())
   
    for (auto _ : state) 
   {
      double ff = 0;
    state.PauseTiming();
    engine.seed(state.range(1));
    std::set<int> data = genRandomSet(state.range(1),state.range(0) );
    state.ResumeTiming();
    // add all data to 'ff'
    for(int v: data)
      ff += v;  // benchmark::DoNotOptimize(...)
    benchmark::ClobberMemory();
   }
}
BENCHMARK(double_plus_assign_rand)
    ->Args({1, 0}) // 1 iter - seed 0
    //->Args({10, 0}) // 10 iter - seed 0
    //->Args({50, 0}) // 50 iter - seed 0
;


static void kfloat64_plus_assign_rand(benchmark::State &state)
{
   

  std::default_random_engine engine;
  engine.seed(0);
  std::uniform_int_distribution<int> runif(-1000,+1000);


   //while(state.KeepRunning())
   
    for (auto _ : state) 
   {
      kfloat64 ff = 0;
    state.PauseTiming();
    engine.seed(state.range(1));
    std::set<int> data = genRandomSet(state.range(1),state.range(0) );
    state.ResumeTiming();
    // add all data to 'ff'
    for(int v: data)
      ff += v; // benchmark::DoNotOptimize(...)
     benchmark::ClobberMemory();
   }
}
BENCHMARK(kfloat64_plus_assign_rand)
    ->Args({1, 0}) // 1 iter - seed 0
    //->Args({10, 0}) // 10 iter - seed 0
    //->Args({50, 0}) // 50 iter - seed 0
;

// templated benchmarks
template <class F> 
static void t_plus_assign(benchmark::State &state)
{
   for (auto _ : state) 
   {
      F f = 0; // accumulator
    for(int k=0; k<state.range(0); k++)
      benchmark::DoNotOptimize(f += k);
    benchmark::ClobberMemory();
   }
}

BENCHMARK_TEMPLATE(t_plus_assign, float)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, kfloat32)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, nfloat32)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, double)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, kfloat64)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, nfloat64)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, long double)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, kfloat128)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign, nfloat128)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

// templated benchmarks
template <class F> 
static void t_plus_assign_final_clobber(benchmark::State &state)
{
   for (auto _ : state) 
   {
      F f = 0; // accumulator
      for(int k=0; k<state.range(0); k++)
        f += k;
      F f2;
      benchmark::DoNotOptimize(f2 = f);
      benchmark::ClobberMemory();
   }
}


BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, double)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, kfloat64)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, nfloat64)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, long double)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, kfloat128)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
;

BENCHMARK_TEMPLATE(t_plus_assign_final_clobber, nfloat128)
   ->Args({1, 0}) // 1 iter - seed 0
   ->Args({16, 0}) // 16 iter - seed 0
   ->Args({64, 0}) // 64 iter - seed 0
*/