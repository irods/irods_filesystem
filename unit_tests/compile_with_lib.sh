#! /bin/bash

source_files='main.cpp path.cpp'

clang++ \
    -std=c++17 \
    -nostdinc++ \
    -stdlib=libc++ \
    -pthread \
    -Wall \
    -I/opt/irods-externals/clang6.0-0/include/c++/v1 \
    -I/opt/irods-externals/boost1.67.0-0/include \
    -I/opt/irods-externals/catch22.3.0-0/include \
    -I/usr/include/irods \
    -o example \
    $source_files \
    -lirods_client \
    -lirods_common \
    -lirods_filesystem
