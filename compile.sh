#! /bin/bash

source_files='test/main.cpp src/path.cpp src/filesystem.cpp'

clang++ \
    -std=c++17 \
    -nostdinc++ \
    -stdlib=libc++ \
    -pthread \
    -Wall \
    -I/opt/irods-externals/clang6.0-0/include/c++/v1 \
    -I./include \
    -o example \
    $source_files 
