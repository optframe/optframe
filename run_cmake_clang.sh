#!/bin/bash
cmake -B build -S . -GNinja  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-19   -DCMAKE_CXX_FLAGS="-stdlib=libc++"
cd build && ninja
