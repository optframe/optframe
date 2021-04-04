## FCore-VRP

### Example of VRP structure with ADS

Just open file: `mainVRP.cpp`

To build: `bazel build ...`

To run: `bazel run //:app_VRP`

### general advices for bazel
Finding available packages with bazel.

local components:
bazel query 'attr(visibility, "//visibility:public", ...)'

remote OptFrame packages:
bazel query @OptFrame//...
bazel query 'attr(visibility, "//visibility:public", "@OptFrame//...")'


list label hdrs:
bazel query 'labels(hdrs, ...)'
