#!/bin/bash

set -e

mkdir -p build && cd build
cmake .. && make -j15

cd ..
build/Octopuzzler
