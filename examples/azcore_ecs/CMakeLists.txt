cmake_minimum_required(VERSION 3.10)

include(zstd)
include(rapidjson)
include(rapidxml)
include(cityhash)
include(lua)
include(googletest)
include(azcore)
include(example_app)

list(APPEND modules log4cpp)
cmake_language(CALL MAKE_EXAMPLE_APP azcore_ecs modules)

message(STATUS "!!!!!!!!!!!!!!!!!!!! ${CMAKE_CURRENT_BINARY_DIR} .. ${CMAKE_BUILD_TYPE}")
message(STATUS "!!!!!!!!!!!!!!!!!!!! ${CMAKE_BUILD_TYPE}")
message(STATUS "!!!!!!!!!!!!!!!!!!!! $<CONFIGURATION>")

COPY_ZSTD_DYLIBS(azcore_ecs "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/")
COPY_CITYHASH_DYLIBS(azcore_ecs "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/")
