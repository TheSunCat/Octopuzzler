#!/bin/bash

set -e

mkdir -p build/web && cd build/web
emcmake cmake ../.. && emmake make -j15
emrun Octopuzzler.html

cd ../..
