#!/bin/bash
cmake -B build -S . -GNinja  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-19   -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DUSE_CXX_MODULES=1
cd build && ninja
