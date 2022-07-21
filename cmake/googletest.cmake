cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_DEPENDENCE_PATH}/googletest/include)
link_directories(${CMAKE_DEPENDENCE_PATH}/googletest/lib)

set(LIBRARIES ${LIBRARIES} gtest gmock gtest_main)