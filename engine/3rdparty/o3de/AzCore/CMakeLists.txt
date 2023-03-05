cmake_minimum_required(VERSION 3.10)

# set the project name
set(PROJECT_NAME o3deAzCore)
project(${PROJECT_NAME})

#set(PROJECT_SOURCE_DIR AzCore
#     Platform/Mac)

#FIXME: Platform/Mac/AzCore need make for differend platforms
set(INCLUDE_DIR .
     Platform/Mac)

include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SRC
     AzCore/*.h
     AzCore/*.cpp
     Platform/Mac/*.h
     Platform/Mac/*.cpp
     Platform/Common/UnixLike/*.h
     Platform/Common/UnixLike/*.cpp
     Platform/Common/UnixLikeDefault/*.h
     Platform/Common/UnixLikeDefault/*.cpp
     Platform/Common/Default/*.h
     Platform/Common/Default/*.cpp
     Platform/Common/Apple/*.h
     Platform/Common/Apple/*.cpp
)

#message(STATUS "ATOM !!!! src ${SRC}")

group_files()

include(zstd)
include(rapidjson)
include(rapidxml)
include(cityhash)
include(lua)
include(googletest)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})