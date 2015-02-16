find_package(Boost 1.57.0 REQUIRED COMPONENTS system unit_test_framework REQUIRED) # program_options filesystem unit_test_framework

include_directories(${Boost_INCLUDE_DIRS})

add_executable(HashSetTest HashSetTest.cpp)
target_link_libraries(HashSetTest PegSolitaire ${Boost_LIBRARIES})
add_test(HashSetTest HashSetTest)

add_executable(PegSolitaireTest PegSolitaireTest.cpp)
target_link_libraries(PegSolitaireTest PegSolitaire ${Boost_LIBRARIES})
add_test(PegSolitaireTest PegSolitaireTest)

add_executable(CodeGenerationTest CodeGenerationTest.cpp)
target_link_libraries(CodeGenerationTest PegSolitaire ${Boost_LIBRARIES})
add_test(CodeGenerationTest CodeGenerationTest)
