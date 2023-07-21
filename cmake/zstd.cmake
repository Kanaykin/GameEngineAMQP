cmake_minimum_required(VERSION 3.10)

set(ZSTD_LIB_DIR ${LIBRARIES} "${CMAKE_DEPENDENCE_PATH}/zstd/lib")
include_directories(${CMAKE_DEPENDENCE_PATH}/zstd/include)
link_directories("${ZSTD_LIB_DIR}")

set(LIBRARIES ${LIBRARIES} zstd)

set(ZSTD_DYLIBS 
	${ZSTD_LIB_DIR}/libzstd.dylib
	${ZSTD_LIB_DIR}/libzstd.1.dylib
	${ZSTD_LIB_DIR}/libzstd.1.5.2.dylib
)

function (COPY_ZSTD_DYLIBS TARGET_NAME DEST_PATH)
	foreach(file ${ZSTD_DYLIBS})
    	add_custom_command(
        	TARGET ${TARGET_NAME}
        	COMMAND ${CMAKE_COMMAND} -E copy ${file} ${DEST_PATH}
        	DEPENDS ${file}
        	COMMENT "Processing file: ${file}"
    	)
	endforeach()
endfunction()