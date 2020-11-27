## FCore-BRKGA

### Example of decoder and BRKGA for TSP

Just open file: `mainTSP-brkga.cpp`

To build: `bazel build ...`

To run: `bazel run //:app_TSPbrkga`

### general advices for bazel
Finding available packages with bazel.

local components:
bazel query 'attr(visibility, "//visibility:public", ...)'

remote OptFrame packages:
bazel query @OptFrame//...
bazel query 'attr(visibility, "//visibility:public", "@OptFrame//...")'


list label hdrs:
bazel query 'labels(hdrs, ...)'
