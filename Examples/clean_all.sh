#!/bin/bash

for example in `ls -I makefile | grep -v "sh$"`; do
   # check if it's a directory (ignore local files)
   if [ -d "../Examples/${example}" ]; then
      echo ""
      echo "Clean Example $example"
      echo "cd ../Examples/${example} && make clean"
      (cd ../Examples/${example} && make clean)
      echo "cd ../Examples/${example} && bazel clean"
      (cd ../Examples/${example} && bazel clean)
   fi
done

