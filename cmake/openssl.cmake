cmake_minimum_required(VERSION 3.10)


set(OPENSSL_LIB_DIR ${LIBRARIES} "${CMAKE_DEPENDENCE_PATH}/openssl/usr/local/lib")
link_directories(${OPENSSL_LIB_DIR})

set(OPENSSL_DYLIBS 
	${OPENSSL_LIB_DIR}/libssl.3.dylib
	${OPENSSL_LIB_DIR}/libcrypto.3.dylib
)

function (COPY_OPENSSL_DYLIBS TARGET_NAME DEST_PATH)
	foreach(file ${OPENSSL_DYLIBS})
    	add_custom_command(
        	TARGET ${TARGET_NAME}
        	COMMAND ${CMAKE_COMMAND} -E copy ${file} ${DEST_PATH}
        	DEPENDS ${file}
        	COMMENT "Processing file: ${file}"
    	)
	endforeach()
endfunction()