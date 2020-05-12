#!/bin/bash

for example in `ls -I makefile | grep -v "sh$"`; do
   echo ""
   echo "Example $example"
   echo "cd ../Examples/${example}/tests && make"
   (cd ../Examples/${example}/tests && make)
done
