This is an LTS example from OptFrame: Eternity II

Created around 2012 for an optimization competition, 
this example demonstrates how to use many neighborhood
structures in a VNS-VND algorithm, using fast move 
reevaluation strategies from optframe.

To build it, there are three ways.

1) use cxxbuild

just install cxxbuild (pip install cxxbuild --break-system-packages) and run 'cxxbuild' 

It requires CMake (recommended CMake 4.0.0), Ninja and runs with C++17 standard (GCC 13 or older is enough).

2) use ./run_cmake_clang_etii.sh

This script runs clang with libc++ on c++20 with cmake and ninja

3) use bazel

Just install bazelisk from NVM/NPM and run: bazel build ...

License

LGPLv3 or MIT, at your choice
