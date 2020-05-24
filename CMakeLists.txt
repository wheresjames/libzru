#====================================================================
#
#   Configure the build
# $ cmake . -B ./out  -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#
#   Execute build
# $ cmake --build ./out -j 8
#
#====================================================================

#====================================================================
# Minimum version
cmake_minimum_required(VERSION 3.10)

# Don't allow in builds in the source tree
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed, try cmake --build ./out")
endif()


#====================================================================
# Project info

# set(PROJECT_NAME "libzru")

# add_compile_options(-DPROJECT_NAME="${PROJECT_NAME}")

# Establish a version number
# execute_process(COMMAND bash -c "date +'%y.%m.%d.%H%M%S'" OUTPUT_VARIABLE BUILD_VERSION)
# string(STRIP "${BUILD_VERSION}" BUILD_VERSION)
# message("VERSION : ${BUILD_VERSION}")
# execute_process(COMMAND bash -c "echo \'${BUILD_VERSION}\' > version.txt")
# add_compile_options(-DBUILD_VERSION="${BUILD_VERSION}")

# Project information
# project(${PROJECT_NAME}
#         VERSION         "${BUILD_VERSION}"
#         DESCRIPTION     "${PROJECT_NAME}"
#         )

project("${APPNAME}" VERSION "${APPVER}")
add_compile_options(-DAPPNAME="${PROJECT_NAME}")
add_compile_options(-DAPPNAMERAW=${PROJECT_NAME})
add_compile_options(-DAPPVER="${PROJECT_VERSION}")
add_compile_options(-DAPPBUILD="${APPBUILD}")


#====================================================================
# Options
add_definitions("-std=c++17")
option(BuildApps "BuildApps" OFF)

#====================================================================
# Setup Conan
include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


#====================================================================
# Include directories
include_directories("src/main/headers")


#====================================================================
# Dependencies
add_subdirectory("src/main")
add_subdirectory("src/test")

if (BuildApps)
    add_subdirectory("src/apps/memmon")
endif()

