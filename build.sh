#!/bin/sh
mkdir -p build && cd build
cmake ..
make -j2$(cat /proc/cpuinfo| grep "processor"| wc -l)
