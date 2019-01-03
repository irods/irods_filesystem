# iRODS Filesystem
An experimental implementation of the C++17 Filesystem library for iRODS.

It can be used on both the client and server side.

This library does not support relative paths when interacting with iRODS objects. All paths must be absolute before any interaction with iRODS objects.

**This library is a work in progress and has NOT been fully tested!**

## Requirements
- iRODS v4.3.0 or greater
- irods-dev package
- irods-runtime package
- irods-externals-boost package
- irods-externals-catch2 package

## Building the Library
To build, just follow the normal steps for building any simple CMake-based project.
```bash
mkdir _build && cd _build
cmake -GNinja /path/to/irods_filesystem
ninja package
```

Once built, you should have a new package (deb or rpm depending on your OS). Once installed, you'll have access to new header files and two shared libraries.
- `libirods_filesystem_client.so` (For client-side code)
- `libirods_filesystem_server.so` (For server-side code)

If your code requires the server-side lib, make sure to define one of the following macros. At least one of these macros must be defined in order to enable the server-side API. The following macros achieve the same result in regards to this library but, they could affect code outside of this library differently.
- RODS_SERVER
- RODS_CLERVER

## TODOs
- Finish designing and implementing unit tests.
- Implement remaining filesystem functions.
  + resize_data_object
  + relative (and siblings; e.g. proximate, canonical)
