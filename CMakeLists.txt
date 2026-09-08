cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mwlib LANGUAGES CXX)
project(mwlib LANGUAGES C)

add_subdirectory("${CMAKE_SOURCE_DIR}/nwlib")

# set(NWLIB_BUILD_TESTS TRUE)
if (NWLIB_BUILD_TESTS)
add_subdirectory("${CMAKE_SOURCE_DIR}/nwlib_testing")
endif()
