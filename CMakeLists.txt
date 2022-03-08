cmake_minimum_required(VERSION 3.10)

project(GameEngineAMQP)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_DEPENDENCE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/dependence")

#add_subdirectory (engine)
add_subdirectory (examples)
