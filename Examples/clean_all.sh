#!/bin/bash

for example in `ls -I makefile | grep -v "sh$"`; do
   echo ""
   echo "Clean Example $example"
   echo "cd ../Examples/${example} && make clean"
   (cd ../Examples/${example} && make clean)
done

