include (GenerateExportHeader)

find_package(Boost 1.57.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)
find_package(LLVM)

file(GLOB_RECURSE SOURCES *.hpp *.cpp)

add_library(PegSolitaire ${LIB_TYPE} ${SOURCES})

target_include_directories(PegSolitaire PUBLIC
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

target_compile_options(PegSolitaire PUBLIC
    "-std=c++14"
    "-Wall"
    "-Wextra"
    "-pedantic"
    ${LLVM_CPPFLAGS}
    ${LLVM_LDFLAGS})

llvm_map_components_to_libnames(LLVM_LIBS support core jit)

target_link_libraries(PegSolitaire PUBLIC
    ${LLVM_LIBS})

generate_export_header(PegSolitaire
    BASE_NAME PegSolitaire
    EXPORT_MACRO_NAME PEGSOLITAIRE_EXPORT
    EXPORT_FILE_NAME PegSolitaire_export.h
    STATIC_DEFINE PEGSOLITAIRE_BUILT_AS_STATIC)
