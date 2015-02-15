add_subdirectory(main)

if(PEGSOLITAIRE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(PEGSOLITAIRE_BUILD_EXPERIMENTS)
    add_subdirectory(experiments)
endif()
if(PEGSOLITAIRE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# include (GenerateExportHeader)

# add_compiler_export_flags(PegSolitaire_EXPORT_FLAGS)
# ps_add_library(PegSolitaire ${LIB_TYPE}
#   Board.cpp
#   BoardIO.cpp
#   BoardCompiler.cpp
#   AST.cpp
#   CodeGenerator.cpp
#   )

# set_target_properties(PegSolitaire PROPERTIES
#   LINK_FLAGS ${PegSolitaire_EXPORT_FLAGS})

# generate_export_header(PegSolitaire
#              BASE_NAME PegSolitaire
#              EXPORT_MACRO_NAME PEGSOLITAIRE_EXPORT
#              EXPORT_FILE_NAME PegSolitaire_export.h
#              STATIC_DEFINE PEGSOLITAIRE_BUILT_AS_STATIC)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include_directories(${CMAKE_CURRENT_BINARY_DIR})

# add_executable(peg-solitaire main.cpp)
# target_link_libraries(peg-solitaire PegSolitaire)

# # TODO integrate the following experiments into the main project
# ps_add_executable(LlvmExperiments LlvmExperiments.cpp)
# ps_add_executable(BoostExperiments BoostExperiments.cpp)
