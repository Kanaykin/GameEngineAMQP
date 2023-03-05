cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_ROOT_PATH}/engine/3rdparty/o3de/AzCore)
include_directories(${CMAKE_ROOT_PATH}/engine/3rdparty/o3de/AzCore/Platform/Mac)
set(LIBRARIES ${LIBRARIES} o3deAzCore)

