#!/bin/bash

for example in `ls -I makefile | grep -v "sh$"`; do
   echo ""
   echo "Make Example $example"
   echo "cd ../Examples/${example} && make"
   (cd ../Examples/${example} && make)
done

