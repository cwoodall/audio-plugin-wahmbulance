#!/usr/bin/env bash
./scripts/build.sh

cd build
ctest --output-on-failure -j4