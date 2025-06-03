This is an LTS example from OptFrame: OptHS

This is OptHouse House Scheduling problem, an utility
tool to share jobs among friend sharing a house.
It was meant to really solve a practical problem and was
code really quickly, in just a few hours.
This was created around 2012 and then 
upgraded multiple times, since v1 to current v6.
Code is quite chaotic, since it was upgraded to 
meet all the evolution of optframe. This is kept
for historical reasons, but it's best to start
from a clean and fresh OptHSv6 example.

To build it, there are two ways.

1) use cxxbuild

just install cxxbuild (pip install cxxbuild --break-system-packages) and run 'cxxbuild' 

It requires CMake (recommended CMake 4.0.0), Ninja and runs with C++20 standard.

2) use bazel

Just install bazelisk from NVM/NPM and run: bazel build ...

License

LGPLv3 or MIT, at your choice
