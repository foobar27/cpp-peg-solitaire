cmake_minimum_required(VERSION 2.8.11.2)
project(PegSolitaire)

cmake_policy(SET CMP0022 NEW)

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(PEGSOLITAIRE_BUILD_TESTS "Build the test suite" ON)
option(PEGSOLITAIRE_BUILD_BENCHMARKS "Build the experiments" ON)
option(PEGSOLITAIRE_BUILD_EXPERIMENTS "Build the experiments" ON)

if (PEGSOLITAIRE_BUILD_TESTS)
    enable_testing()
endif ()


option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
    set (LIB_TYPE SHARED)
endif(BUILD_SHARED_LIBS)

add_subdirectory(src)

# function(ps_add_library TARGET)
#   add_library(${TARGET} ${LIB_TYPE}
#     ${ARGN})
#   target_link_libraries(${TARGET}
#     ${TARGET}
#     ${LLVM_ALL_LIBS}
#     ${Boost_LIBRARIES})
#   set_target_properties(${TARGET} PROPERTIES
#     COMPILE_FLAGS ${LLVM_CPPFLAGS}
#     INTERFACE_LINK_LIBRARIES ${LLVM_LDFLAGS})
# endfunction()

# function(ps_add_executable TARGET)
#   add_executable(${TARGET} ${ARGN})
#   target_link_libraries(${TARGET}
#     PegSolitaire
#     ${LLVM_ALL_LIBS}
#     ${Boost_LIBRARIES})
#   set_target_properties(${TARGET} PROPERTIES
#     COMPILE_FLAGS ${LLVM_CPPFLAGS}
#     LINK_FLAGS ${LLVM_LDFLAGS})
# endfunction()

# add_subdirectory(src)
# if (PEGSOLITAIRE_BUILD_TESTS)
#   add_subdirectory(test)
# endif ()
# if (PEGSOLITAIRE_BUILD_BENCHMARKS)
#   add_subdirectory(benchmarks)
# endif ()
