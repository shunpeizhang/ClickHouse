option (USE_INTERNAL_GRPC_LIBRARY "Set to FALSE to use system grpc library instead of bundled" ${NOT_UNBUNDLED})


if(USE_INTERNAL_GRPC_LIBRARY)
	if(NOT GRPC_ROOT_DIR)
		set(GRPC_ROOT_DIR ${PROJECT_SOURCE_DIR}/third_party/grpc)
	endif()

	if(EXISTS "${GRPC_ROOT_DIR}/CMakeLists.txt")		
		set(GRPC_INCLUDE_DIR "${GRPC_ROOT_DIR}/include")
		set(GRPC_LIBRARY grpc++ grpc++_reflection)
		set(GRPC_FOUND 1)
    else()
      message(WARNING "submodule third_party/rpc is missing. to fix try run: \n git submodule update --init --recursive")
    endif()
endif() 

if (USE_INTERNAL_GRPC_LIBRARY)
    message (STATUS "Using capnp=${ENABLE_GRPC}: ${GRPC_INCLUDE_DIR} : ${GRPC_LIBRARY}")
else ()
    message (STATUS "Build without grpc (support for grpc will be disabled)")
endif ()

