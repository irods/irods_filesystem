cmake_minimum_required(VERSION 3.7.0)

find_package(IRODS REQUIRED)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)

project(irods_filesystem_tester LANGUAGES CXX C)

include(${IRODS_TARGETS_PATH})

if (NOT IRODS_EXTERNALS_PACKAGE_ROOT)
    set(IRODS_EXTERNALS_PACKAGE_ROOT "/opt/irods-externals" CACHE STRING "Choose the location of iRODS external packages." FORCE)
    message(STATUS "Setting unspecified IRODS_EXTERNALS_PACKAGE_ROOT to '${IRODS_EXTERNALS_PACKAGE_ROOT}'")
endif()

set(TARGET irods_filesystem_tester)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/src/path.cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/src/iterators.cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/src/permissions.cpp)

set(INCLUDE_DIRS ${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1
                 ${IRODS_EXTERNALS_FULLPATH_CATCH2}
                 ${IRODS_INCLUDE_DIRS})

set(LIBS irods_client
         irods_common
         irods_plugin_dependencies
         irods_filesystem
         c++abi)

set(CMAKE_COMPILE_COMMANDS TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib
                        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")

add_executable(${TARGET} ${SOURCE})
target_compile_options(${TARGET} PRIVATE -std=c++17 -nostdinc++ -pthread)
target_compile_definitions(${TARGET} PRIVATE ${IRODS_COMPILE_DEFINITIONS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${LIBS})
