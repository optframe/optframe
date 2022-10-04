#!/bin/bash

bazel run   //:app_bTSPfcore
bazel run  --action_env=CC=g++-10 //:app_TSPfxcore


