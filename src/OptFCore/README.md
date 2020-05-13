
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

## Test it

With this simple setup, you can already use any available (neighborhood-based) heuristic/metaheuristic:

- Simulated Annealing
- Hill Climbing
- Iterated Local Search
- Variable Neighborhood Search
- Late Acceptance Hill Climbing
- Tabu Search
- ...

This can be done in the same way as OptFrame v4 heuristic components, see examples for more ;)

**Good luck!**
