


aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(./etcd DIR_LIB_SRCS)
aux_source_directory(./etcd/etcdGrpc DIR_LIB_SRCS)

add_library(CoordinationClient ${DIR_LIB_SRCS})
target_link_libraries(CoordinationClient ${GRPC_LIBRARY})
target_include_directories(CoordinationClient PUBLIC ${GRPC_INCLUDE_DIR} ${Poco_INCLUDE_DIRS})

