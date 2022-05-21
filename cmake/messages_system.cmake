cmake_minimum_required(VERSION 3.10)

include(log)
include_directories(${CMAKE_ROOT_PATH}/engine/messages_system/src)
set(LIBRARIES ${LIBRARIES} messages_system)

