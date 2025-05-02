This is an LTS example from OptFrame: HFM

This is one of the latest examples, a highly complex
fuzzy forecasting system developed by Vitor Nazário.
This was created around 2016 and then 
upgraded multiple times, since v1 to current v6.
Code is quite chaotic, since it was upgraded to 
meet all the evolution of optframe. This is kept
for historical reasons, but it's best to start
from a clean and fresh HFMv6 example.

To build it, there are two ways.

1) use cxxbuild

just install cxxbuild (pip install cxxbuild --break-system-packages) and run 'cxxbuild' 

It requires CMake (recommended CMake 4.0.0), Ninja and runs with C++20 standard.

2) use bazel

Just install bazelisk from NVM/NPM and run: bazel build ...

License

LGPLv3 or MIT, at your choice
