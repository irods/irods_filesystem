#! /bin/bash

FS_LD_LIBRARY_PATH=/opt/irods-externals/clang6.0-0/lib
FS_LD_LIBRARY_PATH=/opt/irods-externals/boost1.67.0-0/lib:${FS_LD_LIBRARY_PATH}
FS_LD_LIBRARY_PATH=$PWD/_build:${FS_LD_LIBRARY_PATH}

LD_LIBRARY_PATH=$FS_LD_LIBRARY_PATH ./example
