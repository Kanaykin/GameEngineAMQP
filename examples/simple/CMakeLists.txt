cmake_minimum_required(VERSION 3.10)

# set the project name
project(simple)

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
include(rabbitmq)
include(amqpcpp)

set(PROJECT_SOURCE_DIR src)

file(GLOB_RECURSE SRC
     ${PROJECT_SOURCE_DIR}/*.h
     ${PROJECT_SOURCE_DIR}/*.cpp
)

add_executable(helloworld ${SRC})
target_link_libraries(helloworld ${LIBRARIES})
