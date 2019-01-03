cmake_minimum_required(VERSION 3.7.0)

find_package(IRODS REQUIRED)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)

project(irods_filesystem LANGUAGES CXX C)

include(${IRODS_TARGETS_PATH})

if (NOT CPACK_PACKAGING_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/" CACHE STRING "Package root path. \"/\" is correct for normal package builds.." FORCE)
    message(STATUS "Setting unspecified CPACK_PACKAGING_INSTALL_PREFIX to '${CPACK_PACKAGING_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build {Debug, Release}." FORCE)
    message(STATUS "Setting unspecified CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'")
endif()

if (NOT IRODS_EXTERNALS_PACKAGE_ROOT)
    set(IRODS_EXTERNALS_PACKAGE_ROOT "/opt/irods-externals" CACHE STRING "Choose the location of iRODS external packages." FORCE)
    message(STATUS "Setting unspecified IRODS_EXTERNALS_PACKAGE_ROOT to '${IRODS_EXTERNALS_PACKAGE_ROOT}'")
endif()

set(TARGET irods_filesystem)

set(TARGET_VERSION_MAJOR 0)
set(TARGET_VERSION_MINOR 0)
set(TARGET_VERSION_PATCH 1)
set(TARGET_VERSION "${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${TARGET_VERSION_PATCH}")

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

set(INCLUDE_DIRS ${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1
                 ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
                 ${IRODS_INCLUDE_DIRS}
                 ${CMAKE_SOURCE_DIR}/include)

set(LIBS -L${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib
         -L${IRODS_EXTERNALS_FULLPATH_BOOST}/lib
         irods_common
         irods_plugin_dependencies
         boost_filesystem
         boost_system
         c++abi)

set(CMAKE_COMPILE_COMMANDS TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib
                        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")

set(TARGET_CLIENT ${TARGET}_client)
set(TARGET_SERVER ${TARGET}_server)

# Client Library
add_library(${TARGET_CLIENT} SHARED ${HEADERS} ${SOURCE})
set_target_properties(${TARGET_CLIENT} PROPERTIES VERSION ${TARGET_VERSION} SOVERSION ${TARGET_VERSION_MAJOR})
target_compile_options(${TARGET_CLIENT} PRIVATE -std=c++17 -nostdinc++ -pthread)
target_compile_definitions(${TARGET_CLIENT} PRIVATE ${IRODS_COMPILE_DEFINITIONS})
target_include_directories(${TARGET_CLIENT} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET_CLIENT} PRIVATE ${LIBS} irods_client)

# Server Library
add_library(${TARGET_SERVER} SHARED ${HEADERS} ${SOURCE})
set_target_properties(${TARGET_SERVER} PROPERTIES VERSION ${TARGET_VERSION} SOVERSION ${TARGET_VERSION_MAJOR})
target_compile_options(${TARGET_SERVER} PRIVATE -std=c++17 -nostdinc++ -pthread)
target_compile_definitions(${TARGET_SERVER} PRIVATE ${IRODS_COMPILE_DEFINITIONS} RODS_SERVER)
target_include_directories(${TARGET_SERVER} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET_SERVER} PRIVATE ${LIBS} irods_server)

install(TARGETS ${TARGET_CLIENT} ${TARGET_SERVER} LIBRARY DESTINATION /usr/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/irods DESTINATION /usr/include)

set(LIBRARY_PACKAGE_NAME irods-filesystem)

set(IRODS_LIBRARY_VERSION_MAJOR ${TARGET_VERSION_MAJOR})
set(IRODS_LIBRARY_VERSION_MINOR ${TARGET_VERSION_MINOR})
set(IRODS_LIBRARY_VERSION_PATCH ${TARGET_VERSION_PATCH})
set(IRODS_LIBRARY_VERSION "${IRODS_LIBRARY_VERSION_MAJOR}.${IRODS_LIBRARY_VERSION_MINOR}.${IRODS_LIBRARY_VERSION_PATCH}")

set(CPACK_PACKAGE_FILE_NAME "${LIBRARY_PACKAGE_NAME}-${IRODS_LIBRARY_VERSION}-${IRODS_LINUX_DISTRIBUTION_NAME}${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_VERSION ${IRODS_LIBRARY_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${IRODS_LIBRARY_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${IRODS_LIBRARY_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${IRODS_LIBRARY_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Renaissance Computing Institute <info@irods.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The integrated Rule-Oriented Data System")
set(CPACK_PACKAGE_VENDOR "Renaissance Computing Institute <info@irods.org>")

set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://irods.org")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_BREAKS ${LIBRARY_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_REPLACES ${LIBRARY_PACKAGE_NAME})

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_VENDOR "iRODS Consortium")
set(CPACK_RPM_PACKAGE_URL "https://irods.org")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)
set(CPACK_RPM_PACKAGE_OBSOLETES ${LIBRARY_PACKAGE_NAME})

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(LIBRARY_PACKAGE_DEPENDENCIES "irods-server (= ${IRODS_VERSION}), irods-runtime (= ${IRODS_VERSION})")

set(CPACK_DEBIAN_PACKAGE_NAME ${LIBRARY_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING}, ${LIBRARY_PACKAGE_DEPENDENCIES}, libc6")

set(CPACK_RPM_PACKAGE_NAME ${LIBRARY_PACKAGE_NAME})
if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos" OR
    IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos linux" OR
    IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "opensuse")
    set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, ${LIBRARY_PACKAGE_DEPENDENCIES}")
endif()

if (NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR ${IRODS_CPACK_GENERATOR} CACHE STRING "CPack generator to use, e.g. {DEB, RPM, TGZ}." FORCE)
    message(STATUS "Setting unspecified CPACK_GENERATOR to ${CPACK_GENERATOR}. This is the correct setting for normal builds.")
endif()

include(CPack)
