## Solver-KP

### Example of Solver Structure for Knapsack Problem

#### Main Build

Just open file: `mainSolverKP.cpp`

To build: `make`

#### Main Build with Bazel 

To build: `bazel build ...`

To run: `./bazel-bin/app_KP01_generic --config default_kp01.json`

#### Solver Library via Bazel

See files: `src/solver_kp01_generic.h` and `solver_kp01_generic.cpp`.

### general advices for bazel
Finding available packages with bazel.

local components:
bazel query 'attr(visibility, "//visibility:public", ...)'

remote OptFrame packages:
bazel query @OptFrame//...
bazel query 'attr(visibility, "//visibility:public", "@OptFrame//...")'


list label hdrs:
bazel query 'labels(hdrs, ...)'
