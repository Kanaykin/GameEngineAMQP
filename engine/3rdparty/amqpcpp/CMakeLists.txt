cmake_minimum_required(VERSION 2.8.12)

project(amqpcpp)

set(CMAKE_MACOSX_RPATH 1)

include_directories("${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include")
link_directories("${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/lib")

message(STATUS "CMAKE_DEPENDENCE_PATH: ${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(
    amqpcpp
    SHARED
    src/AMQP.cpp
    src/AMQPBase.cpp
    src/AMQPException.cpp
    src/AMQPMessage.cpp
    src/AMQPExchange.cpp
    src/AMQPQueue.cpp
    include/AMQPcpp.h
)
target_include_directories(amqpcpp PUBLIC include/ 
    ${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include/)

add_library(
    amqpcpp-static
    STATIC
    src/AMQP.cpp
    src/AMQPBase.cpp
    src/AMQPException.cpp
    src/AMQPMessage.cpp
    src/AMQPExchange.cpp
    src/AMQPQueue.cpp
    include/AMQPcpp.h
)
target_include_directories(amqpcpp-static PUBLIC include/ 
    ${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include/)

#add_subdirectory(rabbitmq-c)
#target_include_directories(rabbitmq PRIVATE ${CMAKE_DEPENDENCE_PATH}/rabbitmq-c/include/)

#target_link_libraries(amqpcpp rabbitmq ssl crypto)
#target_link_libraries(amqpcpp-static rabbitmq ssl crypto)

target_link_libraries(amqpcpp rabbitmq)
target_link_libraries(amqpcpp-static rabbitmq)

install (TARGETS amqpcpp amqpcpp-static
         ARCHIVE DESTINATION ${CMAKE_DEPENDENCE_PATH}/amqpcpp/lib
         LIBRARY DESTINATION ${CMAKE_DEPENDENCE_PATH}/amqpcpp/lib
         RUNTIME DESTINATION ${CMAKE_DEPENDENCE_PATH}/amqpcpp/bin)

install(FILES include/AMQPcpp.h
    DESTINATION ${CMAKE_DEPENDENCE_PATH}/amqpcpp/include)