#!/bin/bash
mkdir build
cd build
make
mv Outrospection ..
cd ..
./Outrospection
