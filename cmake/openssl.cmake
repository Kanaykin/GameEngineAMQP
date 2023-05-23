cmake_minimum_required(VERSION 3.10)


#include_directories(${CMAKE_DEPENDENCE_PATH}/zstd/include)

set(OPENSSL_LIB_DIR ${LIBRARIES} "${CMAKE_DEPENDENCE_PATH}/openssl/usr/local/lib")
link_directories(${OPENSSL_LIB_DIR})

set(OPENSSL_DYLIBS 
	${OPENSSL_LIB_DIR}/libssl.3.dylib
	${OPENSSL_LIB_DIR}/libcrypto.3.dylib
)

#set(LIBRARIES ${LIBRARIES} zstd)