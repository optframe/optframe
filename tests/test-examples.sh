#!/bin/bash

for example in `ls ../Examples/ -I makefile`; do
   echo ""
   echo "Example $example"
   echo "cd ../Examples/${example}/tests && make"
   (cd ../Examples/${example}/tests && make)
done
