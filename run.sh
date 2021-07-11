#!/bin/bash
mkdir build
cd build
make || exit 1
mv Outrospection ..
cd ..
./Outrospection
