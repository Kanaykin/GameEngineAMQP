cmake_minimum_required(VERSION 3.10)

# set the project name
set(PROJECT_NAME o3deAtom)
project(${PROJECT_NAME})

set(PROJECT_SOURCE_DIR RHI/Code/Source)

set(INCLUDE_DIR RHI/Code/Include)

include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SRC
     ${PROJECT_SOURCE_DIR}/*.h
     ${PROJECT_SOURCE_DIR}/*.cpp
)

#message(STATUS "ATOM !!!! src ${SRC}")

group_files()

add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

set_target_properties (o3deAtom PROPERTIES
    FOLDER 3rdparty/o3de
)