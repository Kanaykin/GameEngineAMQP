cmake_minimum_required(VERSION 3.12)

project(GameEngineAMQP)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_DEPENDENCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependence")
set (CMAKE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set (CMAKE_BUILD_PATH "${CMAKE_ROOT_PATH}/build")

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Activate the folders in a solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory (engine)
add_subdirectory (examples)
