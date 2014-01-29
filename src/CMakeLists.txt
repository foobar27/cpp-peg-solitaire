include (GenerateExportHeader)

find_package(Boost 1.55.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)

add_compiler_export_flags(PegSolitaire_EXPORT_FLAGS)
add_library(PegSolitaire ${LIB_TYPE}
#  AST.cpp
  Board.cpp
  BoardIO.cpp
  BoardCompiler.cpp
  )

set_target_properties(PegSolitaire PROPERTIES
  LINK_FLAGS ${PegSolitaire_EXPORT_FLAGS})

generate_export_header(PegSolitaire
             BASE_NAME PegSolitaire
             EXPORT_MACRO_NAME PEGSOLITAIRE_EXPORT
             EXPORT_FILE_NAME PegSolitaire_export.h
             STATIC_DEFINE PEGSOLITAIRE_BUILT_AS_STATIC)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(peg-solitaire main.cpp)
target_link_libraries(peg-solitaire PegSolitaire)

find_package(LLVM)

# TODO integrate the following experiments into the main project
add_executable(LlvmExperiments LlvmExperiments.cpp)
target_link_libraries(LlvmExperiments ${LLVM_ALL_LIBS})
set_target_properties(LlvmExperiments PROPERTIES
  COMPILE_FLAGS ${LLVM_CPPFLAGS}
  LINK_FLAGS ${LLVM_LDFLAGS})

add_executable(BoostExperiments BoostExperiments.cpp AST.cpp CodeGenerator.cpp)
target_link_libraries(BoostExperiments ${LLVM_ALL_LIBS})
set_target_properties(BoostExperiments PROPERTIES
  COMPILE_FLAGS ${LLVM_CPPFLAGS}
  LINK_FLAGS ${LLVM_LDFLAGS})
