cmake_minimum_required(VERSION 3.10)

set(CITYHASH_LIB_DIR "${CMAKE_DEPENDENCE_PATH}/cityhash/lib")
include_directories(${CMAKE_DEPENDENCE_PATH}/cityhash/include)
link_directories("${CITYHASH_LIB_DIR}")

set(LIBRARIES ${LIBRARIES} cityhash)

set(CITYHASH_DYLIBS 
	${CITYHASH_LIB_DIR}/libcityhash.dylib
	${CITYHASH_LIB_DIR}/libcityhash.0.dylib
)

function (COPY_CITYHASH_DYLIBS TARGET_NAME DEST_PATH)
	foreach(file ${CITYHASH_DYLIBS})
    	add_custom_command(
        	TARGET ${TARGET_NAME}
        	COMMAND ${CMAKE_COMMAND} -E copy ${file} ${DEST_PATH}
        	DEPENDS ${file}
        	COMMENT "Processing file: ${file}"
    	)
	endforeach()
endfunction()