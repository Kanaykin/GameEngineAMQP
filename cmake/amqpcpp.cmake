cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_DEPENDENCE_PATH}/amqpcpp/include)
link_directories(${CMAKE_DEPENDENCE_PATH}/amqpcpp/lib)

set(LIBRARIES ${LIBRARIES} amqpcpp)