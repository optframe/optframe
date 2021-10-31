#!/bin/bash

for demo in `ls -I makefile | grep -v "sh$"`; do
   # check if it's a directory (ignore local files)
   if [ -d "../demo/${demo}" ]; then
      echo ""
      echo "Clean Demo $demo"
      echo "cd ../demo/${demo} && make clean"
      (cd ../demo/${demo} && make clean)
      echo "cd ../demo/${demo} && bazel clean"
      (cd ../demo/${demo} && bazel clean)
   fi
done

