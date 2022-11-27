#!/usr/bin/env bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" .
cmake --build build --config DEBUG --parallel 4
