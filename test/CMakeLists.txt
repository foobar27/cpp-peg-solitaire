include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

ps_add_executable(PegSolitaireTest PegSolitaireTest.cpp)
add_test(PegSolitaireTest PegSolitaireTest)

ps_add_executable(HashSetTest HashSetTest.cpp)
add_test(HashSetTest HashSetTest)
