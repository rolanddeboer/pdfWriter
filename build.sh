#!/bin/bash

cd build
cmake -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12 .. && cmake --build . && cmake --install . --component pdfWriter
# cmake .. && cmake --build . && cmake --install . --component pdfWriter
sudo cp -r pdfWriter /usr/local/bin