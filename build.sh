#!/bin/bash
bazel build --copt="-DRAMULATOR" //ramulator:main
bazel build --copt="-DRAMULATOR" //ramulator:testram
bazel build --copt="-DRAMULATOR" //ramulator:testram_break
