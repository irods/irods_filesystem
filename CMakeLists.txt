cmake_minimum_required(VERSION 3.7.0)
project(libirods_filesystem LANGUAGES CXX C)

set(TARGET irods_filesystem)

set(HEADERS ${CMAKE_SOURCE_DIR}/include/irods/filesystem.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/collection_entry.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/collection_iterator.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/recursive_collection_iterator.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/detail.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/filesystem.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/filesystem_error.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/object_status.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/path.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/path_traits.hpp
            ${CMAKE_SOURCE_DIR}/include/irods/filesystem/permissions.hpp)

set(SOURCE ${CMAKE_SOURCE_DIR}/src/filesystem.cpp
           ${CMAKE_SOURCE_DIR}/src/path.cpp
           ${CMAKE_SOURCE_DIR}/src/collection_iterator.cpp
           ${CMAKE_SOURCE_DIR}/src/recursive_collection_iterator.cpp)

set(INCLUDE_DIRS /opt/irods-externals/clang6.0-0/include/c++/v1
                 /opt/irods-externals/boost1.67.0-0/include
                 /usr/include/irods
                 ${CMAKE_SOURCE_DIR}/include)

set(LIBS -L/opt/irods-externals/boost1.67.0-0/lib
         -L/opt/irods-externals/clang6.0-0/lib
         irods_client
         irods_server
         irods_common
         irods_plugin_dependencies
         boost_filesystem
         boost_system
         c++abi)

set(CMAKE_COMPILE_COMMANDS TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH /opt/irods-externals/clang6.0-0/lib
                        /opt/irods-externals/boost1.67.0-0/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")

add_library(${TARGET} SHARED ${HEADERS} ${SOURCE})
target_compile_options(${TARGET} PRIVATE -std=c++17 -nostdinc++ -pthread)
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${LIBS})
