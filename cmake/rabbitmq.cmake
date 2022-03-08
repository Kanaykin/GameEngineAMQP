cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include)
link_directories(${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/lib)

set(LIBRARIES ${LIBRARIES} rabbitmq)

