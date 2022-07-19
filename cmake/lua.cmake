cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_DEPENDENCE_PATH}/lua/include)
link_directories(${CMAKE_DEPENDENCE_PATH}/lua/lib)

set(LIBRARIES ${LIBRARIES} lua)