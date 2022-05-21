cmake_minimum_required(VERSION 3.10)


include_directories(${CMAKE_DEPENDENCE_PATH}/log4cpp/include)
link_directories("${CMAKE_DEPENDENCE_PATH}/log4cpp/lib")

set(LIBRARIES ${LIBRARIES} log4cpp)