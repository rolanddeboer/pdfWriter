#!/bin/bash

platform="$1"

if [[ $platform == "mac" ]]; then
  rm -f makefile
  cp -f makefile-mac makefile
  make
  exit
fi

if [[ $platform == "linux" ]]; then
  rm -f makefile
  cp -f makefile-linux makefile
  make
  exit
fi

echo "Usage: make.sh [platform]. Platform can be mac or linux."