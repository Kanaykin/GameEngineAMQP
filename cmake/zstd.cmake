cmake_minimum_required(VERSION 3.10)


include_directories(${CMAKE_DEPENDENCE_PATH}/zstd/include)
link_directories("${CMAKE_DEPENDENCE_PATH}/zstd/lib")

set(LIBRARIES ${LIBRARIES} zstd)