cmake_minimum_required(VERSION 3.10)

# set the project name
set(PROJECT_NAME messages_system)
project(${PROJECT_NAME})

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
include(log)
include(qpid-proton)
include(group_files)
include(boost)

set(PROJECT_SOURCE_DIR src)
include_directories(src)

file(GLOB_RECURSE SRC
     ${PROJECT_SOURCE_DIR}/*.h
     ${PROJECT_SOURCE_DIR}/*.cpp
)

group_files()

add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
