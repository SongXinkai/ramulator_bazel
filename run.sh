#!/bin/bash
./bazel-bin/ramulator/testram ./configs/DDR3-config.cfg $1
./bazel-bin/ramulator/main ./configs/DDR3-config.cfg $1
