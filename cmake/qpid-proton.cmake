cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_DEPENDENCE_PATH}/qpid-proton/include)
link_directories(${CMAKE_DEPENDENCE_PATH}/qpid-proton/lib)

find_package (CyrusSASL)
find_package (Libuv)

message(STATUS "CyrusSASL_FOUND !!! ${CyrusSASL_LIBRARIES}")
message(STATUS "Libuv_FOUND !!! ${Libuv_LIBRARY}")

set(LIBRARIES ${LIBRARIES} qpid-proton-core-static qpid-proton-proactor-static qpid-proton-static qpid-proton-cpp-static ${CyrusSASL_LIBRARIES} ${Libuv_LIBRARY})
