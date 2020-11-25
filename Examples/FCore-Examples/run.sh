#!/bin/bash

bazel run   //:app_TSPfcore
bazel run  --action_env=CC=g++-10 //:app_TSPfxcore


