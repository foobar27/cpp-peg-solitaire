find_package(Boost 1.55.0 COMPONENTS system program_options filesystem unit_test_framework REQUIRED)
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)
add_executable(testsuite testsuite.cpp)
target_link_libraries(testsuite PegSolitaire ${Boost_LIBRARIES})
add_test(testsuite testsuite)
