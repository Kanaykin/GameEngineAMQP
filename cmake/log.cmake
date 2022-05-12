cmake_minimum_required(VERSION 3.10)

include(log4cpp)
include_directories(${CMAKE_ROOT_PATH}/engine/log/src)
set(LIBRARIES ${LIBRARIES} log)

