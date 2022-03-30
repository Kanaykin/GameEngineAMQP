cmake_minimum_required(VERSION 3.10)

link_directories("${CMAKE_DEPENDENCE_PATH}/qpid-proton/lib")
link_directories("/usr/local/Cellar/openssl@3/3.0.2/lib")

include_directories(${CMAKE_DEPENDENCE_PATH}/qpid-proton/include)

find_package (CyrusSASL)
find_package (Libuv)

if (NOT CyrusSASL_FOUND)
  message(FATAL_ERROR "Cannot find the CyrusSASL ")
endif()

if (NOT Libuv_FOUND)
  message(FATAL_ERROR "Cannot find the Libuv ")
endif()

set(LIBRARIES ${LIBRARIES} qpid-proton-core-static qpid-proton-proactor-static qpid-proton-static qpid-proton-cpp-static ${CyrusSASL_LIBRARIES} ${Libuv_LIBRARY} crypto ssl)
