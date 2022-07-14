cmake_minimum_required(VERSION 3.10)


include_directories(${CMAKE_DEPENDENCE_PATH}/cityhash/include)
link_directories("${CMAKE_DEPENDENCE_PATH}/cityhash/lib")

set(LIBRARIES ${LIBRARIES} cityhash)