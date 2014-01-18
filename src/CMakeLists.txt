include (GenerateExportHeader)

find_package(Boost 1.55.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)
find_package(LLVM)

add_compiler_export_flags(PegSolitaire_EXPORT_FLAGS)
add_library(PegSolitaire ${LIB_TYPE} Board.cpp BoardIO.cpp BoardBuilder.cpp)
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

if (PEGSOLITAIRE_USE_LLVM)
  # TODO integrate the following experiments into the main project
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CPPFLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")
  add_executable(llvm-experiments LlvmExperiments.cpp)
  target_link_libraries(llvm-experiments ${LLVM_ALL_LIBS})
endif ()
