cmake_minimum_required(VERSION 2.8.11.2)
project(PegSolitaire)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_definitions(-std=c++11 -Wall -pedantic)

option(PEGSOLITAIRE_BUILD_TESTS "Build the test suite" ON)
option(PEGSOLITAIRE_USE_LLVM "Allow runtime code generation via LLVM." ON)

OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)


if (PEGSOLITAIRE_BUILD_TESTS)
  enable_testing()
endif ()

add_subdirectory(src)
if (PEGSOLITAIRE_BUILD_TESTS)
  add_subdirectory(test)
endif ()
