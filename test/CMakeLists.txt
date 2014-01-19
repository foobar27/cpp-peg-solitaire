find_package(Boost 1.55.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

add_executable(PegSolitaireTest PegSolitaireTest.cpp)
target_link_libraries(PegSolitaireTest PegSolitaire ${Boost_LIBRARIES})
add_test(PegSolitaireTest PegSolitaireTest)

add_executable(HashSetTest HashSetTest.cpp)
target_link_libraries(HashSetTest PegSolitaire ${Boost_LIBRARIES})
add_test(HashSetTest HashSetTest)
