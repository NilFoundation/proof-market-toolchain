 #!/bin/bash

mkdir -p build && cd ./build && cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-12 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 .. && cmake --build . -t proof-generator

