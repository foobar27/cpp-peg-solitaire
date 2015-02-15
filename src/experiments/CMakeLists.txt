add_executable(main main.cpp)
target_link_libraries(main PegSolitaire)

add_executable(BoostExperiments BoostExperiments.cpp)
target_link_libraries(BoostExperiments PegSolitaire)

#add_executable(LlvmExperiments LlvmExperiments.cpp)
#target_link_libraries(LlvmExperiments PegSolitaire)

add_executable(PhoenixExperiments PhoenixExperiments.cpp)
target_link_libraries(PhoenixExperiments PegSolitaire)

#add_executable(ProtoExperiments ProtoExperiments.cpp)
#target_link_libraries(ProtoExperiments PegSolitaire)
