## FCore-PSO

### Example of Particle Swarm Optimization

Just open file: `mainPSO.cpp`

To build: `bazel build ...`

To run: `bazel run //:app_PSO`

### general advices for bazel
Finding available packages with bazel.

local components:
bazel query 'attr(visibility, "//visibility:public", ...)'

remote OptFrame packages:
bazel query @OptFrame//...
bazel query 'attr(visibility, "//visibility:public", "@OptFrame//...")'


list label hdrs:
bazel query 'labels(hdrs, ...)'
