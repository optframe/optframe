This is an LTS example from OptFrame: TSP

This is the oldest one, created around 2008 and then 
upgraded multiple times, since v1 to current v6.
Code is quite chaotic, since it was upgraded to 
meet all the evolution of optframe. This is kept
for historical reasons, but it's best to start
from a clean and fresh TSPv6 example.

To build it, there are three ways.

1) use cxxbuild

just install cxxbuild (pip install cxxbuild --break-system-packages) and run 'cxxbuild' 

It requires CMake (recommended CMake 4.0.0), Ninja and runs with C++17 standard (GCC 13 or older is enough).

2) use ./run_cmake_clang_tsp.sh

This script runs clang with libc++ on c++20 with cmake and ninja

3) use bazel

Just install bazelisk from NVM/NPM and run: bazel build ...

License

LGPLv3 or MIT, at your choice
