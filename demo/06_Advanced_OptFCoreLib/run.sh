#!/bin/bash
ln -s build/liboptfxcore.so liboptfxcore.so 
export LD_LIBRARY_PATH=. &&  ./appTestLib 

