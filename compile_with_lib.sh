#! /bin/bash

source_files='test/main.cpp'

clang++ \
    -std=c++17 \
    -nostdinc++ \
    -stdlib=libc++ \
    -pthread \
    -Wall \
    -I/opt/irods-externals/clang6.0-0/include/c++/v1 \
    -I/usr/include/irods \
    -I./include \
    -I/opt/irods-externals/boost1.67.0-0/include \
    -L./_build\
    -o example \
    $source_files \
    -lirods_client \
    -lirods_common \
    -lirods_filesystem
