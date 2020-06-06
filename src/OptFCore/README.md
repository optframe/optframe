
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

Using `stack` allocation of move (it still allocates heap on "move undo" return, if not optimized out...):
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

Move apply (and undo) itself seems to be a problem (`./build/FCore_bench --benchmark_filter=SimpleMove`):
```
-------------------------------------------------------------------------------
Benchmark                                     Time             CPU   Iterations
-------------------------------------------------------------------------------
TSP_SimpleMove_CPP/10/0/2/5                 437 ns          439 ns      1569705
TSP_SimpleMove_CPP/20/0/2/5                 451 ns          453 ns      1562250
TSP_SimpleMove_CPP/30/0/2/5                 453 ns          455 ns      1442971
TSP_SimpleMove_CPP_std_swap/10/0/2/5        468 ns          471 ns      1515963
TSP_SimpleMove_CPP_std_swap/20/0/2/5        432 ns          435 ns      1502256
TSP_SimpleMove_CPP_std_swap/30/0/2/5        442 ns          444 ns      1603412
TSP_SimpleMove_MoveXF/10/0/2/5              534 ns          538 ns      1278361
TSP_SimpleMove_MoveXF/20/0/2/5              536 ns          540 ns      1303314
TSP_SimpleMove_MoveXF/30/0/2/5              538 ns          542 ns      1284274
TSP_SimpleMove_new_uptr/10/0/2/5            486 ns          489 ns      1455610
TSP_SimpleMove_new_uptr/20/0/2/5            486 ns          489 ns      1421294
TSP_SimpleMove_new_uptr/30/0/2/5            496 ns          499 ns      1444948
```

### Complete Summary (hardcoded vs generic)

```
---------------------------------------------------------------------------------------------------------
Benchmark                                                               Time             CPU   Iterations
---------------------------------------------------------------------------------------------------------
TSP_hardcoded_CPP_Swap_iteration/10/0                                 559 ns          560 ns      1248880
TSP_hardcoded_CPP_Swap_iteration/20/0                                 936 ns          935 ns       746098
TSP_hardcoded_CPP_Swap_iteration/30/0                                1584 ns         1579 ns       440731
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/10/0            3004 ns         3005 ns       233375
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/20/0           10925 ns        10935 ns        63965
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapFancy>/30/0           24056 ns        24076 ns        29498
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/10/0           2833 ns         2835 ns       247837
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/20/0          10230 ns        10236 ns        67430
TSP_generic_TestNSSeq_Swap_iteration<NSSeqSwapBoring>/30/0          22655 ns        22667 ns        31010
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/10/0                     572 ns          572 ns      1227251
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/20/0                     967 ns          966 ns       728538
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/30/0                    1675 ns         1670 ns       420721
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/10/0       1302 ns         1303 ns       535281
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/20/0       3773 ns         3773 ns       183462
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/30/0       7953 ns         7951 ns        86843
TSP_generic_IMS_uptr_apply_NSSeqBoring_Swap_iteration/10/0           2610 ns         2610 ns       270011
TSP_generic_IMS_uptr_apply_NSSeqBoring_Swap_iteration/20/0           9264 ns         9267 ns        74508
TSP_generic_IMS_uptr_apply_NSSeqBoring_Swap_iteration/30/0          20427 ns        20433 ns        34179
TSP_hardcoded_CPP_apply_stack_Swap_iteration/10/0                    1893 ns         1894 ns       370908
TSP_hardcoded_CPP_apply_stack_Swap_iteration/20/0                    6320 ns         6322 ns       106113
TSP_hardcoded_CPP_apply_stack_Swap_iteration/30/0                   13764 ns        13769 ns        50473
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/10/0                  551 ns          550 ns      1278376
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/20/0                  866 ns          863 ns       811936
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/30/0                 1445 ns         1439 ns       486877
TSP_generic_CPP_stackOpCurrent_Swap_iteration/10/0                   2133 ns         2134 ns       327260
TSP_generic_CPP_stackOpCurrent_Swap_iteration/20/0                   7535 ns         7538 ns        91813
TSP_generic_CPP_stackOpCurrent_Swap_iteration/30/0                  16654 ns        16661 ns        41642
TSP_generic_CPP_stackCurrent_Swap_iteration/10/0                     1907 ns         1908 ns       366618
TSP_generic_CPP_stackCurrent_Swap_iteration/20/0                     6380 ns         6384 ns       108804
TSP_generic_CPP_stackCurrent_Swap_iteration/30/0                    13887 ns        13891 ns        50185
TSP_generic_CPP_stackXCurrent_Swap_iteration/10/0                    4391 ns         4395 ns       159725
TSP_generic_CPP_stackXCurrent_Swap_iteration/20/0                   16642 ns        16650 ns        41470
TSP_generic_CPP_stackXCurrent_Swap_iteration/30/0                   37394 ns        37406 ns        18673
TSP_SimpleMove_CPP/10/0/2/5                                           433 ns          436 ns      1625536
TSP_SimpleMove_CPP/20/0/2/5                                           434 ns          436 ns      1565609
TSP_SimpleMove_CPP/30/0/2/5                                           433 ns          435 ns      1607609
TSP_SimpleMove_CPP_std_swap/10/0/2/5                                  435 ns          437 ns      1602435
TSP_SimpleMove_CPP_std_swap/20/0/2/5                                  450 ns          452 ns      1565579
TSP_SimpleMove_CPP_std_swap/30/0/2/5                                  438 ns          441 ns      1596346
TSP_SimpleMove_MoveXF/10/0/2/5                                        532 ns          536 ns      1268565
TSP_SimpleMove_MoveXF/20/0/2/5                                        535 ns          538 ns      1249484
TSP_SimpleMove_MoveXF/30/0/2/5                                        531 ns          535 ns      1254509
TSP_SimpleMove_new_uptr/10/0/2/5                                      476 ns          478 ns      1458763
TSP_SimpleMove_new_uptr/20/0/2/5                                      487 ns          490 ns      1455322
TSP_SimpleMove_new_uptr/30/0/2/5                                      478 ns          481 ns      1450440
```

Got some amazing results with moves purely as functions (pure-C++ fast):
```
---------------------------------------------------------------------------------------------
Benchmark                                                   Time             CPU   Iterations
---------------------------------------------------------------------------------------------
TSP_SimpleMove_NoUndo_CPP_std_swap/10/0/2/5               439 ns          442 ns      1595984
TSP_SimpleMove_NoUndo_CPP_std_swap/20/0/2/5               442 ns          445 ns      1594212
TSP_SimpleMove_NoUndo_CPP_std_swap/30/0/2/5               439 ns          442 ns      1565460
TSP_SimpleMove_NoUndo_new_uptr/10/0/2/5                   481 ns          485 ns      1450483
TSP_SimpleMove_NoUndo_new_uptr/20/0/2/5                   480 ns          483 ns      1435259
TSP_SimpleMove_NoUndo_new_uptr/30/0/2/5                   477 ns          481 ns      1449503
TSP_SimpleMove_NoUndo_MyMoveX_nocopy/10/0/2/5             476 ns          480 ns      1360861
TSP_SimpleMove_NoUndo_MyMoveX_nocopy/20/0/2/5             476 ns          479 ns      1273014
TSP_SimpleMove_NoUndo_MyMoveX_nocopy/30/0/2/5             480 ns          483 ns      1393144
TSP_SimpleMove_NoUndo_MyMoveX_func/10/0/2/5               471 ns          475 ns      1462063
TSP_SimpleMove_NoUndo_MyMoveX_func/20/0/2/5               473 ns          477 ns      1397714
TSP_SimpleMove_NoUndo_MyMoveX_func/30/0/2/5               473 ns          477 ns      1465777
TSP_SimpleMove_NoUndo_MyMoveX_func_noundo/10/0/2/5        444 ns          447 ns      1598340
TSP_SimpleMove_NoUndo_MyMoveX_func_noundo/20/0/2/5        441 ns          445 ns      1579256
TSP_SimpleMove_NoUndo_MyMoveX_func_noundo/30/0/2/5        441 ns          445 ns      1574482
```

Approach `TSP_SimpleMove_NoUndo_MoveFuncList_apply` seems to be the way to go.. spends few `ns` (`2-8ns`) more than others (such as `TSP_SimpleMove_NoUndo_MyMoveX_func_noundo`).

-------

In terms of `constexpr`, it doesn't seem to change anything:

```
TSP_SimpleMove_NoUndo_MoveFuncList_apply/10/0/2/5                 433 ns          436 ns      1606857
TSP_SimpleMove_NoUndo_MoveFuncList_apply/20/0/2/5                 443 ns          446 ns      1571392
TSP_SimpleMove_NoUndo_MoveFuncList_apply/30/0/2/5                 432 ns          435 ns      1607031
TSP_SimpleMove_NoUndo_MoveFuncListConstexpr_apply/10/0/2/5        437 ns          440 ns      1606998
TSP_SimpleMove_NoUndo_MoveFuncListConstexpr_apply/20/0/2/5        434 ns          437 ns      1557600
TSP_SimpleMove_NoUndo_MoveFuncListConstexpr_apply/30/0/2/5        437 ns          440 ns      1592026
```

We can try to generalize `MoveFuncList` approach.

-------

Back to complete tests... still struggling to match CPP baseline. Version `TSP_hardcoded_CPP_MoveUndoFuncList` was supposed to be competitive, but it's not:

```
---------------------------------------------------------------------------------------------------------
Benchmark                                                               Time             CPU   Iterations
---------------------------------------------------------------------------------------------------------
TSP_hardcoded_CPP_Swap_iteration/10/0                                 571 ns          567 ns      1250708
TSP_hardcoded_CPP_Swap_iteration/20/0                                 953 ns          950 ns       750413
TSP_hardcoded_CPP_Swap_iteration/30/0                                1630 ns         1626 ns       437512
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/10/0                     579 ns          575 ns      1233907
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/20/0                     981 ns          979 ns       719683
TSP_hardcoded_IMS_NSSeqBoring_Swap_iteration/30/0                    1695 ns         1692 ns       396484
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/10/0       1315 ns         1312 ns       546820
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/20/0       3891 ns         3890 ns       179632
TSP_hardcoded_IMS_uptr_noapply_NSSeqBoring_Swap_iteration/30/0       8052 ns         8052 ns        87070
TSP_hardcoded_CPP_apply_stack_Swap_iteration/10/0                    1941 ns         1941 ns       372093
TSP_hardcoded_CPP_apply_stack_Swap_iteration/20/0                    6311 ns         6314 ns        99704
TSP_hardcoded_CPP_apply_stack_Swap_iteration/30/0                   13792 ns        13803 ns        50451
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/10/0                  553 ns          552 ns      1282281
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/20/0                  881 ns          878 ns       811419
TSP_hardcoded_CPP_fapply_nostack_Swap_iteration/30/0                 1466 ns         1461 ns       485023
TSP_hardcoded_CPP_MoveUndoFuncList/10/0                              1085 ns         1085 ns       645058
TSP_hardcoded_CPP_MoveUndoFuncList/20/0                              3081 ns         3079 ns       220987
TSP_hardcoded_CPP_MoveUndoFuncList/30/0                              6207 ns         6205 ns       112269
```

### Learn More

This can be done in the same way as OptFrame v4 heuristic components, see examples for more ;)

**Good luck!**
