cmake_minimum_required(VERSION 3.10)

# set the project name
set(PROJECT_NAME peer-to-peer)
project(${PROJECT_NAME})

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
include(messages_system)
include(qpid-proton)
include(group_files)

set(PROJECT_SOURCE_DIR src)

file(GLOB_RECURSE SRC
     ${PROJECT_SOURCE_DIR}/*.h
     ${PROJECT_SOURCE_DIR}/*.cpp
)

group_files()

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_BUILD_PATH}/bin/${CMAKE_BUILD_TYPE})
