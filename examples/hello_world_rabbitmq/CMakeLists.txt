cmake_minimum_required(VERSION 3.10)

# set the project name
set(PROJECT_NAME hello_world_rabbitmq)
project(${PROJECT_NAME})

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
include(rabbitmq)

set(PROJECT_SOURCE_DIR src)

file(GLOB_RECURSE SRC
     ${PROJECT_SOURCE_DIR}/*.h
     ${PROJECT_SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
