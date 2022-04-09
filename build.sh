#!/bin/sh
mkdir -p build && cd build
cmake ..
make -j$(cat /proc/cpuinfo| grep "processor"| wc -l)
