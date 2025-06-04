#!/bin/bash

X=$(uname -a | grep Darwin | grep x86_64)
mkdir -p build/

if [ -z "$X" ]; then
   echo "Not Darwin x86_64! Can use -g"
   c++ --std=c++20 -g -I./include   -Wall -pedantic -O3  --shared  ./src/OptFrameLib/OptFrameLib.cpp -o build/optframe_lib.so -fPIC
else
   echo "On Darwin x86_64! Cannot use -g"
   c++ --std=c++20    -I./include   -Wall -pedantic -O3  --shared  ./src/OptFrameLib/OptFrameLib.cpp -o build/optframe_lib.so -fPIC
fi
