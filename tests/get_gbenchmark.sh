#!/bin/bash

#CMAKE=/usr/local/cmake-3.17.2-Linux-x86_64/bin/cmake
CMAKE=cmake
# refresh submodules
git submodule update --init --recursive
git submodule update --recursive
#
###############
# USING CLANG
#
#(export CXX=/usr/bin/clang++ && cd thirdparty && cd gbenchmark && rm -rf build && mkdir build && cd build && cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_LTO=true .. && make )
# ... /libbenchmark.a: erro ao adicionar símbolos: Formato de ficheiro não reconhecido
#
###############
# USING GCC
#
# LTO not working on any gcc: -DBENCHMARK_ENABLE_LTO=true
(export CXX=/usr/bin/g++ && cd ../libs && cd gbenchmark && rm -rf build && mkdir build && cd build && $CMAKE -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Release .. && make )
