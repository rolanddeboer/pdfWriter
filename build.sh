#!/bin/bash

cd build
cmake .. && cmake --build . && sudo cp pdfWriter /usr/local/bin