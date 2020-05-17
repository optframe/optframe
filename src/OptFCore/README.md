
# OptFCore

## OptFrame Functional Core (FCore)

We are proud to present a functional version of OptFrame (after v4.1), that doesn't rely on dozens of files!

Every project can consist of a single file, being the operations written as lambdas (closures).

## Installation

OptFrame `FCore` can be installed together with OptFrame, default placed on `/usr/local/include/OptFCore`.

Note that it requires newest `c++20` features, such as concepts and coroutines (via flag `-fcoroutines`).

We have successfully tested it on `gcc-10.1` (GCC version `10.1`).

## TSP Example

We present a simplified step-by-step Traveling Salesman Problem (TSP) implementation (also present on `Examples/` folder, a single file `mainTSP-fcore.cpp`).

This simplified examples relies on four main components:

- some `ProblemContext` to read and store TSP instance data (no specific format is required!)
- search space definition, as a pair of representation and objective value types
- an evaluator
- one (or more) constructive methods
- one (or more) neighborhood structures

### Imports
We consider the following imports.
```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <OptFCore/FCore.hpp> // OptFrame Functional Core (FCore)
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/Util/printable.h>
//
using namespace std;
using namespace optframe;
using namespace scannerpp;
```

### ProblemContext

```cpp
class ProblemContext
{
public:
   int n;               // number of clients
   Matrix<double> dist; // distance matrix (Euclidean)
   // load data from Scanner
   void load(Scanner& scanner)
   {
      n = *scanner.nextInt();      //* reads number of clients
      dist = Matrix<double>(n, n); // initializes n x n matrix
      //
      vector<double> xvalues(n);
      vector<double> yvalues(n);
      //
      for (int i = 0; i < n; i++) {
         scanner.next();
         xvalues[i] = *scanner.nextDouble(); //* reads x
         yvalues[i] = *scanner.nextDouble(); //* reads y
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
```

### Search Space definitions

```cpp
// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP = std::pair<
  std::vector<int>,  // first part of search space element: solution (representation)
  Evaluation<double> // second part of search space element: evaluation (objective value)
  >;
```

### Evaluation function

```cpp
using TSPEval = FEvaluator <
                ESolutionTSP,
      true, // minimization: true
  [](auto& s) -> auto
{
   double f = 0;
   for (int i = 0; i < int(pTSP.n) - 1; i++)
      f += pTSP.dist(s[i], s[i + 1]);
   f += pTSP.dist(s[int(pTSP.n) - 1], s[0]);
   return Evaluation<double>{ f };
}
> ;
```

### Random Constructive

```cpp
using TSPRandom = FConstructive <
                  std::vector<int>,
      [](double timelimit) -> auto
{
   vector<int> v(pTSP.n, -1); // get information from pTSP problem
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = i;
   std::random_shuffle(v.begin(), v.end());
   return make_optional(v);
}
> ;
```

### Neighborhood Structure: Swap

A swap move definition:
```cpp
using MoveSwap = FMove<
  std::pair<int, int>,
  ESolutionTSP,
  [](const std::pair<int, int>& moveData, ESolutionTSP& se) -> std::pair<int, int> {
     int i = moveData.first;
     int j = moveData.second;
     // perform swap of clients i and j
     int aux = se.first[j];
     se.first[j] = se.first[i];
     se.first[i] = aux;
     return std::pair<int, int>(j, i); // return a reverse move ('undo' move)
  }>;
```

A neighborhood coroutine enumerator (for all swaps):

```cpp
using NSSeqSwapFancy = FNSSeqFancy<
  ESolutionTSP,
  // random enumerator
  [](const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
     int i = rand() % pTSP.n;
     int j = i;
     while (j <= i) {
        i = rand() % pTSP.n;
        j = rand() % pTSP.n;
     }
     return uptr<Move<ESolutionTSP>>(new MoveSwap{ make_pair(i, j) });
  },
  // coroutine enumerator
  [](const ESolutionTSP& se) -> Generator<Move<ESolutionTSP>*> {
     for (int i = 0; i < int(pTSP.n) - 1; i++)
        for (int j = i + 1; j < pTSP.n; j++)
           co_yield new MoveSwap{ make_pair(i, j) }; // implicit unique_ptr requirements
  }>;
```

Note that `coroutines` provide the `FNSSeqFancy` implementation, but we still have the `FNSSeqBoring` with fallback OptFrame NSIterator strategy. The `FNSSeqBoring` takes dozens of lines more, thus only kept here for benchmarks while `coroutine` support is being improved in time.

## Test it

With this simple setup, you can already use any available (neighborhood-based) heuristic/metaheuristic:

- Simulated Annealing
- Hill Climbing
- Iterated Local Search
- Variable Neighborhood Search
- Late Acceptance Hill Climbing
- Tabu Search
- ...

### Example with Simulated Annealing

```cpp
// load data into 'pTSP'.. only 3 cities (x,y): (10,10) (20,20) (30,30)
Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
pTSP.load(scanner);
// finished load TSP data (it could be from File too...)
TSPEval ev;               // evaluator
TSPRandom crand;          // random constructive
NSSeqSwapFancy swapFancy; // swap neighborhood
//
// testing simulated annealing (also requires a InitialSearch strategy)
BasicInitialSearch<ESolutionTSP> initRand(crand, ev);
RandGen rg;               // Random number generator
// we build a simulated annealing with alpha=0.98, iterMax=100 and temperature=99999
BasicSimulatedAnnealing<ESolutionTSP> sa{
   ev, initRand, swapFancy, 0.98, 100, 99999, rg
};
// will run simulated annealing for 10 seconds
SearchStatus status = sa.search(StopCriteria<ESolutionTSP::second_type>{ 10.0 });
ESolutionTSP best = *sa.getBestSolution();
// best solution value
best.second.print();
```

## Benchmarking

Current benchmarking is available on `FCore` (just type `make bench`).
C++ simple implementation is still many times faster:

```
2020-05-17 18:12:45
Running ./build/FCore_bench
Run on (4 X 3100 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB (x1)
Load Average: 0.94, 0.87, 0.89
---------------------------------------------------------------------------------------------
Benchmark                                                   Time             CPU   Iterations
---------------------------------------------------------------------------------------------
TSP_CPP_Swap_iteration/10/0                               560 ns          560 ns      1249883
TSP_CPP_Swap_iteration/20/0                              1000 ns          998 ns       704922
TSP_CPP_Swap_iteration/30/0                              1603 ns         1602 ns       435393
TSP_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/10/0        3069 ns         3041 ns       233415
TSP_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/20/0       10712 ns        10722 ns        64316
TSP_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/30/0       23716 ns        23730 ns        29637
TSP_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/10/0       2833 ns         2831 ns       248409
TSP_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/20/0      10251 ns        10258 ns        67587
TSP_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/30/0      22440 ns        22452 ns        31030
```

Bad news that it seems some OptFrame internals are very costly. Good news is that coroutines doesn't seem to increase this overhead by much (some `5-10%`), and are so much easier to write.

Interesting is that `uptr` Move apply and undo seems to be responsible for almost all overhead.
This time, `IMS` version uses the complete class, except `current()` move generation,
while the next one uses it.

```
---------------------------------------------------------------------------------------------
Benchmark                                                   Time             CPU   Iterations
---------------------------------------------------------------------------------------------
TSP_CPP_Swap_iteration/10/0                               560 ns          560 ns      1249883
TSP_CPP_Swap_iteration/20/0                              1000 ns          998 ns       704922
TSP_CPP_Swap_iteration/30/0                              1603 ns         1602 ns       435393
TSP_IMS_NSSeqBoring_Swap_iteration/10/0                   570 ns          569 ns      1231731
TSP_IMS_NSSeqBoring_Swap_iteration/20/0                   966 ns          964 ns       727061
TSP_IMS_NSSeqBoring_Swap_iteration/30/0                  1670 ns         1669 ns       419778
TSP_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/10/0       1276 ns         1276 ns       548748
TSP_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/20/0       3722 ns         3722 ns       187490
TSP_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/30/0       7857 ns         7865 ns        88596
TSP_IMS_uptr_apply_NSSeqBoring_Swap_iteration/10/0         2587 ns         2588 ns       270772
TSP_IMS_uptr_apply_NSSeqBoring_Swap_iteration/20/0         9330 ns         9336 ns        73853
TSP_IMS_uptr_apply_NSSeqBoring_Swap_iteration/30/0        20946 ns        20962 ns        34408
```

Using `stack` allocation of move (without undo):
```
TSP_CPP_apply_stack_Swap_iteration/10/0                    1917 ns         1918 ns       362146
TSP_CPP_apply_stack_Swap_iteration/20/0                    6386 ns         6386 ns       107966
TSP_CPP_apply_stack_Swap_iteration/30/0                   13873 ns        13880 ns        50195
```

Issue is not on function move apply either... when avoiding heap, price is short:
```
TSP_CPP_fapply_nostack_Swap_iteration/10/0                  548 ns          548 ns      1269137
TSP_CPP_fapply_nostack_Swap_iteration/20/0                  865 ns          862 ns       814942
TSP_CPP_fapply_nostack_Swap_iteration/30/0                 1479 ns         1475 ns       482831
```


### Learn More

This can be done in the same way as OptFrame v4 heuristic components, see examples for more ;)

**Good luck!**
