cmake_minimum_required(VERSION 2.8.11.2)
project(PegSolitaire)

cmake_policy(SET CMP0022 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Boost 1.55.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)
find_package(LLVM)

add_definitions(-std=c++11 -Wall -pedantic)

option(PEGSOLITAIRE_BUILD_TESTS "Build the test suite" ON)
option(PEGSOLITAIRE_BUILD_BENCHMARKS "Build the benchmarks" ON)

OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

function(ps_add_library TARGET)
  add_library(${TARGET} ${LIB_TYPE}
    ${ARGN})
  target_link_libraries(${TARGET}
    ${TARGET}
    ${LLVM_ALL_LIBS}
    ${Boost_LIBRARIES})
  set_target_properties(${TARGET} PROPERTIES
    COMPILE_FLAGS ${LLVM_CPPFLAGS}
    INTERFACE_LINK_LIBRARIES ${LLVM_LDFLAGS})
endfunction()

function(ps_add_executable TARGET)
  add_executable(${TARGET} ${ARGN})
  target_link_libraries(${TARGET}
    PegSolitaire
    ${LLVM_ALL_LIBS}
    ${Boost_LIBRARIES})
  set_target_properties(${TARGET} PROPERTIES
    COMPILE_FLAGS ${LLVM_CPPFLAGS}
    LINK_FLAGS ${LLVM_LDFLAGS})
endfunction()

if (PEGSOLITAIRE_BUILD_TESTS)
  enable_testing()
endif ()

add_subdirectory(src)
if (PEGSOLITAIRE_BUILD_TESTS)
  add_subdirectory(test)
endif ()
if (PEGSOLITAIRE_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif ()
